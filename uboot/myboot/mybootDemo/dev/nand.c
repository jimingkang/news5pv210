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
	/*��ҳ��д���е�ַ��Ч,NFADDRΪ8λ���ݼĴ���*/
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
	/*���RnB*/
	clear_detect_RnB();
	/*��������ffh*/
	NF_send_cmd(0xff);
	/*�ȴ�RnB*/
	wait_RnB();
	/*chip deselect*/
	deselect_NFchip();
}


void nand_init()
{	
	/*��ʼ��NFCONF*/
	NFCONF |= TACLS|TWRPH0|TWRPH1|(0x0<<3)|(0x0<<2)|(0x1<<1);
	/*��ʼ��NFCONT*/
  NFCONT |= 0x01;
	/*��λnandflash*/
	NF_reset();
}


void NF_page_read(unsigned long addr,unsigned char *buffer)
{
	int i;
	/*select chip*/
	select_NFchip();
	
	/*���RnB �ӵ͵�ƽ���ߵ�ƽ ���ֱ�־*/
	clear_detect_RnB();
	
	/*��������00h*/
	NF_send_cmd(0x00);
	
	/*���͵�ַCol Add.1,2 Row Add.1,2,3 */
	NF_send_addr(addr);
	
	/*��������30h*/
	NF_send_cmd(0x30);
	
	/*wait RnB*/
	wait_RnB();
	
	/*������*/
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
		/*��ַ��ǰ11λΪ�е�ַ����ҳ��ȡ���е�ַ��Ч*/
		unsigned int page_shift = 11;
		for(i=(start_addr>>page_shift);size>0;i++)
		{
			NF_page_read(i,dram);
			size -= PAGESIZE;
			dram += PAGESIZE;
		}
}

/*��nandflash��д����ǰ����Ҫ�Ȳ���nandflash*/
int NF_block_erase(unsigned long addr)
{
	int ret;
	/*chip select*/
	select_NFchip();
	/*���RnB����ת��״̬*/
	clear_detect_RnB();
	/*��������60h*/
	NF_send_cmd(0x60);
	/*����Row Addr.1,2,3*/
	NFADDR = addr&0xff;
	NFADDR = (addr>>8)&0xff;
	NFADDR = (addr>>16)&0xff;
	
	/*��������d0h*/
	NF_send_cmd(0xd0);
	/*�ȴ�RnB*/
	wait_RnB();
	
	/*��������70h*/
	NF_send_cmd(0x70);
	/*��ȡ����I/O�ڣ�0��pass,1:fail*/
	ret = NFDATA;
	/*chip deselect*/
	deselect_NFchip();
	
	return ret;
}


int NF_page_program(unsigned long addr,unsigned char *buf)
{
	int ret;
	int i;
	/*Ƭѡ*/
	select_NFchip();
	/*���RnB����ת��״̬*/
	clear_detect_RnB();
	/*��������80h*/	
	NF_send_cmd(0x80);
	/*���͵�ַCol Addr.1,2 Row Addr.1,2,3*/
	NF_send_addr(addr);
	
	/*д��1 to m byte*/
	for(i=0;i<PAGESIZE;i++)
	{
		NFDATA = buf[i];
	}
	
	/*��������10h*/
	NF_send_cmd(0x10);
	/*�ȴ�RnB*/
	wait_RnB();
	/*��������70h*/
	NF_send_cmd(0x70);
	/*��ȡI/O��*/
	ret = NFDATA;
	/*ȡ��Ƭѡ*/
	deselect_NFchip();
	
	return ret;
}