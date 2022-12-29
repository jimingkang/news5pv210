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
	/*��������*/	
	GPA0CON |= 0x2|(0x2<<4);
	
	/*���ݸ�ʽ����*/
	ULCON0 |= 0x3|(0x0<<2)|(0x0<<3);
	
	/*�շ�����ģʽ���Լ�ʱ��Դ��ѡ��*/
	UCON0  |= 0x1|(0x1<<2)|(0x0<<10);
	
	/*��ֹFIFO������*/
	UFCON0 |= 0x0;
	
	/*������*/
	UMCON0 |= 0x0<<4;
	
	/*����������*/
	UBRDIV0 = 0x23;
	UDIVSLOT0 = 0x888;	
}

void uart0_sendbyte(unsigned char c)
{
	/*��Ϊ��ʱһֱ�ȴ���ֱ�����ͻ�����Ϊ��*/
	while(!(UTRSTAT0&0x1<<2))
		;
	
	UTXH0 = c;
}


unsigned uart0_getbyte()
{
	/*Ϊ��һֱ�ȴ���ֱ�����ܻ�������Ϊ��*/	
	while(!(UTRSTAT0&0x1<<0))
		;
	
	return URXH0;
	
}