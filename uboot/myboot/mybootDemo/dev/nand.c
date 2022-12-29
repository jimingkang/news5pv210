#define NFCONT   (*(volatile unsigned long*) 0xb0e00004)
#define NFSTAT   (*(volatile unsigned long*) 0xb0e00028)
#define NFCMMD   (*(volatile unsigned char*) 0xb0e00008)
#define NFADDR   (*(volatile unsigned char*) 0xb0e0000c)
#define NFDATA   (*(volatile unsigned char*) 0xb0e00010)
#define NFCONF   (*(volatile unsigned long*) 0xb0e00000)


#define TACLS  (0x7<<12)
#define TWRPH0 (0x7<<8)
#define TWRPH1 (0x7<<4)

#define PAGESIZE  2048


void select_NFchip()
{
	NFCONT &= ~(0x1<<1);
}

void deselect_NFchip()
{
	NFCONT |= (0x1<<1);	
}

void clear_detect_RnB()
{
	NFSTAT |= (0x1<<4);	
}

void NF_send_cmd(unsigned char cmd)
{
	NFCMMD = cmd;
}

void NF_send_addr(unsigned long addr)
{
	/*按页读写，列地址无效,NFADDR为8位数据寄存器*/
	NFADDR = 0;	
	NFADDR = 0;
	NFADDR = addr&0xff;
	NFADDR = (addr>>8)&0xff;
	NFADDR = (addr>>16)&0xff;
}

void wait_RnB()
{
	while(!(NFSTAT&(0x1<<4)))
		;
}

void NF_reset()
{
	/*chip select*/	
	select_NFchip();
	/*清除RnB*/
	clear_detect_RnB();
	/*发送命令ffh*/
	NF_send_cmd(0xff);
	/*等待RnB*/
	wait_RnB();
	/*chip deselect*/
	deselect_NFchip();
}


void nand_init()
{	
	/*初始化NFCONF*/
	NFCONF |= TACLS|TWRPH0|TWRPH1|(0x0<<3)|(0x0<<2)|(0x1<<1);
	/*初始化NFCONT*/
  NFCONT |= 0x01;
	/*复位nandflash*/
	NF_reset();
}


void NF_page_read(unsigned long addr,unsigned char *buffer)
{
	int i;
	/*select chip*/
	select_NFchip();
	
	/*清除RnB 从低电平到高电平 发现标志*/
	clear_detect_RnB();
	
	/*发送命令00h*/
	NF_send_cmd(0x00);
	
	/*发送地址Col Add.1,2 Row Add.1,2,3 */
	NF_send_addr(addr);
	
	/*发送命令30h*/
	NF_send_cmd(0x30);
	
	/*wait RnB*/
	wait_RnB();
	
	/*读数据*/
	for(i=0;i<PAGESIZE;i++)
	{
		*buffer++	= NFDATA;
	}
	
	/*deselect chip*/
	deselect_NFchip();
}


void nand_to_ram(unsigned long start_addr,unsigned char *dram,int size)
{
		int i;
		/*地址的前11位为列地址，按页读取，列地址无效*/
		unsigned int page_shift = 11;
		for(i=(start_addr>>page_shift);size>0;i++)
		{
			NF_page_read(i,dram);
			size -= PAGESIZE;
			dram += PAGESIZE;
		}
}

/*对nandflash的写操作前，需要先擦除nandflash*/
int NF_block_erase(unsigned long addr)
{
	int ret;
	/*chip select*/
	select_NFchip();
	/*清除RnB发现转换状态*/
	clear_detect_RnB();
	/*发送命令60h*/
	NF_send_cmd(0x60);
	/*发送Row Addr.1,2,3*/
	NFADDR = addr&0xff;
	NFADDR = (addr>>8)&0xff;
	NFADDR = (addr>>16)&0xff;
	
	/*发送命令d0h*/
	NF_send_cmd(0xd0);
	/*等待RnB*/
	wait_RnB();
	
	/*发送命令70h*/
	NF_send_cmd(0x70);
	/*读取数据I/O口，0：pass,1:fail*/
	ret = NFDATA;
	/*chip deselect*/
	deselect_NFchip();
	
	return ret;
}


int NF_page_program(unsigned long addr,unsigned char *buf)
{
	int ret;
	int i;
	/*片选*/
	select_NFchip();
	/*清除RnB发现转换状态*/
	clear_detect_RnB();
	/*发送命令80h*/	
	NF_send_cmd(0x80);
	/*发送地址Col Addr.1,2 Row Addr.1,2,3*/
	NF_send_addr(addr);
	
	/*写入1 to m byte*/
	for(i=0;i<PAGESIZE;i++)
	{
		NFDATA = buf[i];
	}
	
	/*发送命令10h*/
	NF_send_cmd(0x10);
	/*等待RnB*/
	wait_RnB();
	/*发送命令70h*/
	NF_send_cmd(0x70);
	/*读取I/O口*/
	ret = NFDATA;
	/*取消片选*/
	deselect_NFchip();
	
	return ret;
}