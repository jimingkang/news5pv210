#include "dm9000.h"
#include "arp.h"

#define DM_ADD_PORT (*(volatile unsigned short *) 0x88001000)
#define DM_DAT_PORT (*(volatile unsigned short *) (0x88001000+0x300c))


#define SROM_BW     (*(volatile unsigned long *) 0xE8000000)
#define SROM_BC1    (*(volatile unsigned long *) 0xE8000008)

#define MP0_1CON    (*(volatile unsigned long *) 0xE02002E0)

#define GPH0CON     (*(volatile unsigned long *) 0xE0200C00)
#define EXT_INT_0_CON    (*(volatile unsigned long *) 0xE0200E00)
#define EXT_INT_0_MASK   (*(volatile unsigned long *) 0xE0200F00)
#define EXT_INT_0_PEND   (*(volatile unsigned long *) 0xE0200F40)

#define VIC0INTSELECT    (*(volatile unsigned long *) 0xf200000c)
#define VIC0INTENABLE    (*(volatile unsigned long *) 0xF2000010)
#define VIC0VECTADDR7    (*(volatile unsigned long *) 0xF200011C)


#define VIC0ADDRESS     (*(volatile unsigned long*) 0xf2000f00)
#define VIC1ADDRESS     (*(volatile unsigned long*) 0xf2100f00)
#define VIC2ADDRESS  		(*(volatile unsigned long*) 0xF2200F00)
#define VIC3ADDRESS  		(*(volatile unsigned long*) 0xF2300F00)



u8 host_mac_addr[6] = {0xff,0xff,0xff,0xff,0xff,0xff};
u8 mac_addr[6] = {9,8,7,6,5,4};
u8 ip_addr[4] = {192,168,0,33};
u8 host_ip_addr[4] = {192,168,0,1};
unsigned  short packet_len;
unsigned char buffer[1500];


void dm9000_reg_write(unsigned short addr,unsigned short data)
{
	DM_ADD_PORT = addr;
	DM_DAT_PORT = data;	
}

unsigned char dm9000_reg_read(unsigned short addr)
{
	DM_ADD_PORT = addr;
	return DM_DAT_PORT;	
}


void dm9000_set_CS()
{
	unsigned int tmp;
	
	/*data width is 16bit*/
	SROM_BW &= ~(0xf<<4);
	SROM_BW |= 0x1<<4;
	
	/*TIMING*/
	SROM_BC1 = (0x0<<28)|(0x0<<24)|(0x5<<16)|(0x0<<12)|(0x0<<8)|(0x0<<4)|(0x0<<0);
	
	/*引脚复用为SROM_CS[1]*/
	tmp = MP0_1CON;
	tmp &=~(0xf<<4);								
	tmp |=(2<<4);
	MP0_1CON = tmp;
	
}



void dm9000_int_init()
{
	
	
	/*引脚初始化*/	
	GPH0CON &= (~(0xf<<28));
	GPH0CON |= (0xf<<28);
	
/*以下内容可以加入irq_init,dm9000_int_init只进行引脚初始化*/
#if 0	
	/*高电平触发*/
	EXT_INT_0_CON &= (~(0x7<<28));
	EXT_INT_0_CON |= (0x1<<28);


	/*Mask设置*/
	EXT_INT_0_MASK = 0x0;
	
	/*mode=irq*/	
	VIC0INTSELECT &= (~(1<<7));


	/*中断使能*/
	VIC0INTENABLE |= (0x1<<7);

		
	/*中断向量地址*/
	VIC0VECTADDR7 = (int)dm9000_isr;
#endif
}

void  dm9000_reset()
{
  dm9000_reg_write(DM9000_GPCR, GPCR_GPIO0_OUT);
  dm9000_reg_write(DM9000_GPR, 0);	
  
  dm9000_reg_write(DM9000_NCR, (NCR_LBK_INT_MAC | NCR_RST));
  dm9000_reg_write(DM9000_NCR, 0);
  
  dm9000_reg_write(DM9000_NCR, (NCR_LBK_INT_MAC | NCR_RST));
  dm9000_reg_write(DM9000_NCR, 0);		
}

void dm9000_probe()
{
	unsigned int id_val;
	id_val = dm9000_reg_read(DM9000_VIDL);
	id_val |= dm9000_reg_read(DM9000_VIDH) << 8;
	id_val |= dm9000_reg_read(DM9000_PIDL) << 16;
	id_val |= dm9000_reg_read(DM9000_PIDH) << 24;
	if (id_val == DM9000_ID) {
		printf("dm9000 is found !\r\n");
		return ;
	} else {
		printf("dm9000 is not found !\r\n");
		return ;
	}	
}

void dm9000_init()
{
	unsigned int i;
	
	/*set CS*/
	dm9000_set_CS();

	/*INT INIT*/
	dm9000_int_init();
#if 1		
	/*reset device*/
	 dm9000_reset();
	 
	/*Search DM9000 board, allocate space and register it*/
	dm9000_probe();
	
	/*MAC INIT*/
	/* Program operating register, only internal phy supported */
	dm9000_reg_write(DM9000_NCR, 0x0);
	/* TX Polling clear */
	dm9000_reg_write(DM9000_TCR, 0);
	/* Less 3Kb, 200us */
	dm9000_reg_write(DM9000_BPTR, BPTR_BPHW(3) | BPTR_JPT_600US);
	/* Flow Control : High/Low Water */
	dm9000_reg_write(DM9000_FCTR, FCTR_HWOT(3) | FCTR_LWOT(8));
	/* SH FIXME: This looks strange! Flow Control */
	dm9000_reg_write(DM9000_FCR, 0x0);
	/* Special Mode */
	dm9000_reg_write(DM9000_SMCR, 0);
	/* clear TX status */
	dm9000_reg_write(DM9000_NSR, NSR_WAKEST | NSR_TX2END | NSR_TX1END);
	/* Clear interrupt status */
	dm9000_reg_write(DM9000_ISR, ISR_ROOS | ISR_ROS | ISR_PTS | ISR_PRS);
	
	/*fill device MAC address registers*/
  for (i = 0; i < 6; i++)
    dm9000_reg_write(DM9000_PAR+i, mac_addr[i]);
	
	/*Activate DM9000*/
	//dm9000_reg_write(DM9000_RCR, RCR_DIS_LONG | RCR_ALL | RCR_DIS_CRC | RCR_RXEN);  /*RCR_ALL为1开启广播包,0过滤广播包*/
	dm9000_reg_write(DM9000_RCR, RCR_DIS_LONG | RCR_DIS_CRC | RCR_RXEN);
	/* Enable TX/RX interrupt mask */
	dm9000_reg_write(DM9000_IMR, IMR_PAR);
#endif
}



void dm9000_tx(unsigned char *data,unsigned int length)
{
  unsigned int i;
  
  /*禁止中断*/
  dm9000_reg_write(DM9000_IMR,0x80);
  
  /*写入发送数据的长度*/
  dm9000_reg_write(DM9000_TXPLL, length & 0xff);
  dm9000_reg_write(DM9000_TXPLH, (length >> 8) & 0xff);
  
  /*写入待发送的数据*/
  DM_ADD_PORT = DM9000_MWCMD;
 
  for(i=0;i<length;i+=2)
  {
  	DM_DAT_PORT = data[i] | (data[i+1]<<8);
  }
  
  /*启动发送*/
  dm9000_reg_write(DM9000_TCR, TCR_TXREQ); 
  
  /*等待发送结束*/
  while(1)
  {
     unsigned char status;
     status = dm9000_reg_read(DM9000_TCR);
     if((status&0x01)==0x00)
         break;	
  }
  
  /*清除发送状态*/
  dm9000_reg_write(DM9000_NSR,0x2c);
  
  /*恢复中断使能*/
  dm9000_reg_write(DM9000_IMR,0x81);
}

#define PTK_MAX_LEN 1522

unsigned short dm9000_rx(unsigned char *data)
{
  unsigned short status,len;
  unsigned short tmp;
  unsigned int i;
  unsigned char ready = 0;
  
  /*判断是否产生中断，且清除*/
  if(dm9000_reg_read(DM9000_ISR) & 0x01)
      dm9000_reg_write(DM9000_ISR,0x01);
  else
      return 0;
      
  /*空读*/
  ready = dm9000_reg_read(DM9000_MRCMDX);
  
  if ((ready & 0x01) != 0x01)
  {
   	ready = dm9000_reg_read(DM9000_MRCMDX);
    if ((ready & 0x01) != 0x01)
    	 return 0;
  }
  
  /*读取状态*/
  status = dm9000_reg_read(DM9000_MRCMD);
  
  /*读取包长度*/
  len = DM_DAT_PORT;
  
  /*读取包数据*/
  if(len<PTK_MAX_LEN)
  {
     for(i=0;i<len;i+=2)
     {
         tmp = DM_DAT_PORT;
         data[i] = tmp & 0x0ff;
         data[i+1] = (tmp>>8)&0x0ff;
     }
  }
  
  return len;
}


void dm9000_isr()
{// u8 len=buffer[0];
	packet_len = dm9000_rx(buffer);
	net_handle(&buffer[0],packet_len);
	
	/*清除中断*/	
	EXT_INT_0_PEND = ~0x0;
	
	/*清除中断地址*/
	VIC0ADDRESS = 0;
	VIC1ADDRESS = 0;
	VIC2ADDRESS = 0;
	VIC3ADDRESS = 0;	
}


void dm9000_arp()
{
    while(1)
        arp_request();	
}




