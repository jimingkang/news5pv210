#include "stdio.h"

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




// 初始化ADC控制器的函数
void adc_init(void)
{
	rTSADCCON0	|= (1<<16);		// resolution set to 12bit
	rTSADCCON0	|= (1<<14);		// enable clock prescaler
	rTSADCCON0 	&= ~(0xFF<<6);
	rTSADCCON0	|= (65<<6);		// convertor clock = 66/66M=1MHz, MSPS=200KHz
	rTSADCCON0 	&= ~(1<<2);		// normal operation mode
	rTSADCCON0 	&= ~(1<<1);		// disable start by read mode
	
	rADCMUX		&= ~(0x0F<<0);	// MUX选择ADCIN0
}

// 注意：第一，延时要确实能延时；第二，延时时间必须合适。
static void delay(void)
{
	volatile unsigned int i, j;
	
	for (i=0; i<4000; i++)
		for (j=0; j<1000; j++);
}

void adc_collect1(void)
{
	unsigned int val = 0;
	// 第一步：手工开启ADC转换
	rTSADCCON0	|= (1<<0);
	// 第二步：等待ADC转换完毕
	while (!(rTSADCCON0 & (1<<15)));
	
	// 第三步：读取ADC的数字值
	// 第四步：处理/显示数字值
	val = rTSDATX0;
	printf("x: adc value = %d.\n", (val & (0xFFF<<0)));
}




















