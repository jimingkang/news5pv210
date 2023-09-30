#define GPA0CON   (*(volatile unsigned long*) 0xe0200000)
#define ULCON0    (*(volatile unsigned long*) 0xe2900000)  
#define UCON0     (*(volatile unsigned long*) 0xe2900004)
#define UFCON0    (*(volatile unsigned long*) 0xe2900008)
#define UMCON0    (*(volatile unsigned long*) 0xe290000c)
#define UTRSTAT0  (*(volatile unsigned long*) 0xe2900010)
#define UBRDIV0   (*(volatile unsigned long*) 0xe2900028)
#define UDIVSLOT0 (*(volatile unsigned long*) 0xe290002c)
#define UTXH0     (*(volatile unsigned long*) 0xe2900020)
#define URXH0     (*(volatile unsigned long*) 0xe2900024)

void uart0_init()
{
	/*引脚配置*/	
	GPA0CON |= 0x2|(0x2<<4);
	
	/*数据格式设置*/
	ULCON0 |= 0x3|(0x0<<2)|(0x0<<3);
	
	/*收发工作模式，以及时钟源的选择*/
	UCON0  |= 0x1|(0x1<<2)|(0x0<<10);
	
	/*禁止FIFO缓冲区*/
	UFCON0 |= 0x0;
	
	/*无流控*/
	UMCON0 |= 0x0<<4;
	
	/*波特率设置*/
	UBRDIV0 = 0x23;
	UDIVSLOT0 = 0x888;	
}

void uart0_sendbyte(unsigned char c)
{
	/*不为空时一直等待，直到发送缓冲区为空*/
	while(!(UTRSTAT0&0x1<<2))
		;
	
	UTXH0 = c;
}


unsigned uart0_getbyte()
{
	/*为空一直等待，直到接受缓冲区不为空*/	
	while(!(UTRSTAT0&0x1<<0))
		;
	
	return URXH0;
	
}