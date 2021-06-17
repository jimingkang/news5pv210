#include "stdio.h"
#include "int.h"
#include "main.h"

void uart_init(void);


void delay(int i)
{
	volatile int j = 10000;
	while (i--)
		while(j--);
}


int main(void)
{
	uart_init();

	
	// 如果程序中要使用中断，就要调用中断初始化来初步初始化中断控制器
	//system_init_exception();
	
	printf("-------------adc test--------------\n");
	adc_test();
	
	// 绑定isr到中断控制器硬件
	//intc_setvectaddr(NUM_WDT, isr_wdt);

	
	// 使能中断
	//intc_enable(NUM_WDT);

	
	while (1);

	return 0;
}