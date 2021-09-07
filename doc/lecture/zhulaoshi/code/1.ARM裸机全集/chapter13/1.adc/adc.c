#include "main.h"

#define TSADCCON0		0xE1700000
#define TSDATX0			0xE170000C
#define TSDATY0			0xE1700010
//#define CLRINTADC0		0xE1700000
#define ADCMUX			0xE170001C

#define rTSADCCON0		(*(volatile unsigned int *)TSADCCON0)
#define rTSDATX0		(*(volatile unsigned int *)TSDATX0)
#define rTSDATY0		(*(volatile unsigned int *)TSDATY0)
//#define rCLRINTADC0		(*(volatile unsigned int *)CLRINTADC0)
#define rADCMUX			(*(volatile unsigned int *)ADCMUX)




// ��ʼ��ADC�������ĺ���
static void adc_init(void)
{
	rTSADCCON0	|= (1<<16);		// resolution set to 12bit
	rTSADCCON0	|= (1<<14);		// enable clock prescaler
	rTSADCCON0 	&= ~(0xFF<<6);
	rTSADCCON0	|= (65<<6);		// convertor clock = 66/66M=1MHz, MSPS=200KHz
	rTSADCCON0 	&= ~(1<<2);		// normal operation mode
	rTSADCCON0 	&= ~(1<<1);		// disable start by read mode
	
	rADCMUX		&= ~(0x0F<<0);	// MUXѡ��ADCIN0
}

// ע�⣺��һ����ʱҪȷʵ����ʱ���ڶ�����ʱʱ�������ʡ�
static void delay(void)
{
	volatile unsigned int i, j;
	
	for (i=0; i<4000; i++)
		for (j=0; j<1000; j++);
}


// ����ADC����ɵĹ��ܾ���ѭ�����ADC���õ�ADCת������ֵ��ӡ����
void adc_test(void)
{
	unsigned int val = 0;
	
	adc_init();
	
	while (1)
	{
		// ��һ�����ֹ�����ADCת��
		rTSADCCON0	|= (1<<0);
		// �ڶ������ȴ�ADCת�����
		while (!(rTSADCCON0 & (1<<15)));
		
		// ����������ȡADC������ֵ
		// ���Ĳ�������/��ʾ����ֵ
		val = rTSDATX0;
		printf("x: bit14 = %d.\n", (val & (1<<14)));
		printf("x: adc value = %d.\n", (val & (0xFFF<<0)));
		
		val = rTSDATY0;
		printf("y: bit14 = %d.\n", (val & (1<<14)));
		printf("y: adc value = %d.\n", (val & (0xFFF<<0)));

		// ���岽����ʱһ��
		delay();
	}
	
}






















