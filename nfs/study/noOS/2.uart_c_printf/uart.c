#define GPA0CON		0xE0200020
#define UCON0 		0xE2900804
#define ULCON0 		0xE2900800
#define UMCON0 		0xE290080C
#define UFCON0 		0xE2900808
#define UBRDIV0 	0xE2900828
#define UDIVSLOT0	0xE290082C
#define UTRSTAT0	0xE2900810
#define UTXH0		0xE2900820
#define URXH0		0xE2900824

#define rGPA0CON	(*(volatile unsigned int *)GPA0CON)
#define rUCON0		(*(volatile unsigned int *)UCON0)
#define rULCON0		(*(volatile unsigned int *)ULCON0)
#define rUMCON0		(*(volatile unsigned int *)UMCON0)
#define rUFCON0		(*(volatile unsigned int *)UFCON0)
#define rUBRDIV0	(*(volatile unsigned int *)UBRDIV0)
#define rUDIVSLOT0	(*(volatile unsigned int *)UDIVSLOT0)
#define rUTRSTAT0		(*(volatile unsigned int *)UTRSTAT0)
#define rUTXH0		(*(volatile unsigned int *)UTXH0)
#define rURXH0		(*(volatile unsigned int *)URXH0)

// ���ڳ�ʼ������
void uart_init(void)
{
	// ��ʼ��Tx Rx��Ӧ��GPIO����
	rGPA0CON &= ~(0xff<<0);			// �ѼĴ�����bit0��7ȫ������
	rGPA0CON |= 0x00000022;			// 0b0010, Rx Tx
	
	// �����ؼ��Ĵ���������
	rULCON0 = 0x3;
	rUCON0 = 0x5;
	rUMCON0 = 0;
	rUFCON0 = 0;
	
	// ����������	DIV_VAL = (PCLK / (bps x 16))-1
	// PCLK_PSYS��66MHz��		����0.8
	//rUBRDIV0 = 34;	
	//rUDIVSLOT0 = 0xdfdd;
	
	// PCLK_PSYS��66.7MHz��		����0.18
	// DIV_VAL = (66700000/(115200*16)-1) = 35.18
	rUBRDIV0 = 35;
	// (rUDIVSLOT�е�1�ĸ���)/16=��һ�����������=0.18
	// (rUDIVSLOT�е�1�ĸ��� = 16*0.18= 2.88 = 3
	rUDIVSLOT0 = 0x0888;		// 3��1����ٷ��Ƽ���õ��������
}


// ���ڷ��ͳ��򣬷���һ���ֽ�
void putc(char c)
{                  	
	// ���ڷ���һ���ַ�����ʵ���ǰ�һ���ֽڶ������ͻ�������ȥ
	// ��Ϊ���ڿ���������1���ֽڵ��ٶ�ԶԶ����CPU���ٶȣ�����CPU����1���ֽ�ǰ����
	// ȷ�ϴ��ڿ�������ǰ�������ǿյģ���˼���Ǵ����Ѿ���������һ���ֽڣ�
	// ����������ǿ���λΪ0����ʱӦ��ѭ����ֱ��λΪ1
	while (!(rUTRSTAT0 & (1<<1)));
	rUTXH0 = c;
}

// ���ڽ��ճ�����ѯ��ʽ������һ���ֽ�
char getc(void)
{
	while (!(rUTRSTAT0 & (1<<0)));
	return (rURXH0 & 0x0f);
}
















