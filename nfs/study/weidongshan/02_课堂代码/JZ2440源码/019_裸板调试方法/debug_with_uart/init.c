#include "s3c24xx.h"

void putc(unsigned char c);
void puts(char *s);
void puthex(unsigned long val);


/* WOTCH DOG register */
#define 	WTCON				(*(volatile unsigned long *)0x53000000)

/* SDRAM regisers */
#define 	MEM_CTL_BASE		0x48000000
 
void disable_watch_dog();
void memsetup();

/*�ϵ��WATCH DOGĬ���ǿ��ŵģ�Ҫ�����ص� */
void disable_watch_dog()
{
	WTCON	= 0;
}

/* ���ÿ���SDRAM��13���Ĵ��� */
void memsetup()
{
	int 	i = 0;
	unsigned long *p = (unsigned long *)MEM_CTL_BASE;
	volatile unsigned long *mem = (volatile unsigned long *)0x30000000;

#if 0
    /* SDRAM 13���Ĵ�����ֵ */
    unsigned long  const    mem_cfg_val[]={ 0x22011110,     //BWSCON
                                            0x00000700,     //BANKCON0
                                            0x00000700,     //BANKCON1
                                            0x00000700,     //BANKCON2
                                            0x00000700,     //BANKCON3  
                                            0x00000700,     //BANKCON4
                                            0x00000700,     //BANKCON5
                                            0x00018005,     //BANKCON6
                                            0x00018005,     //BANKCON7
                                            0x008C07A3,     //REFRESH
                                            0x000000B1,     //BANKSIZE
                                            0x00000030,     //MRSRB6
                                            0x00000030,     //MRSRB7
                                    };

	//puts("start init mem\n\r");
	putc('s');
	putc('t');
	putc('a');
	putc('r');
	putc('t');

	puthex(0x1234abcd);


	for(; i < 13; i++)
	{
		p[i] = mem_cfg_val[i];
		putc('1'+i);
		putc(':');
		puthex(&mem_cfg_val[i]);
		putc(' ');
		puthex(mem_cfg_val[i]);
		putc('\n');
		putc('\r');
	}

	//puts("end init mem\n\r");
	putc('e');
	putc('n');
	putc('d');
#endif
	p[0] = 0x22011110;     //BWSCON	
	p[1] = 0x00000700;     //BANKCON0	
	p[2] = 0x00000700;     //BANKCON1	
	p[3] = 0x00000700;     //BANKCON2	
	p[4] = 0x00000700;     //BANKCON3  	
	p[5] = 0x00000700;     //BANKCON4	
	p[6] = 0x00000700;     //BANKCON5	
	p[7] = 0x00018005;     //BANKCON6	
	p[8] = 0x00018005;     //BANKCON7	
	p[9] = 0x008C07A3;     //REFRESH	
	p[10] = 0x000000B1;     //BANKSIZE	
	p[11] = 0x00000030;     //MRSRB6	
	p[12] = 0x00000030;     //MRSRB7

	*mem = 0x5555aaaa;
	putc('v');
	putc('a');
	putc('l');
	putc(':');
	puthex(*mem);
}

#define TXD0READY   (1<<2)
#define RXD0READY   (1)

#define PCLK            12000000
#define UART_CLK        PCLK        //  UART0��ʱ��Դ��ΪPCLK
#define UART_BAUD_RATE  57600      // ������
#define UART_BRD        ((UART_CLK  / (UART_BAUD_RATE * 16)) - 1)

/*
 * ��ʼ��UART0
 * 57600,8N1,������
 */
void uart0_init(void)
{
    GPHCON  |= 0xa0;    // GPH2,GPH3����TXD0,RXD0
    GPHUP   = 0x0c;     // GPH2,GPH3�ڲ�����

    ULCON0  = 0x03;     // 8N1(8������λ���޽��飬1��ֹͣλ)
    UCON0   = 0x05;     // ��ѯ��ʽ��UARTʱ��ԴΪPCLK
    UFCON0  = 0x00;     // ��ʹ��FIFO
    UMCON0  = 0x00;     // ��ʹ������
    UBRDIV0 = UART_BRD; // ������Ϊ115200
}

/*
 * ����һ���ַ�
 */
void putc(unsigned char c)
{
    /* �ȴ���ֱ�����ͻ������е������Ѿ�ȫ�����ͳ�ȥ */
    while (!(UTRSTAT0 & TXD0READY));
    
    /* ��UTXH0�Ĵ�����д�����ݣ�UART���Զ��������ͳ�ȥ */
    UTXH0 = c;
}

void puts(char *s)
{
	int i = 0;
	while (s[i])
	{
		putc(s[i]);
		i++;
	}
}

void puthex(unsigned long val)
{
	/* val = 0x1234ABCD */
	unsigned char c;
	int i = 0;
	
	putc('0');
	putc('x');

	for (i = 0; i < 8; i++)
	{
		c = (val >> ((7-i)*4)) & 0xf;
		if ((c >= 0) && (c <= 9))
		{
			c = '0' + c;
		}
		else if ((c >= 0xA) && (c <= 0xF))
		{
			c = 'A' + (c -  0xA);
		}
		putc(c);
	}
}

