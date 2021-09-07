#include "stdio.h"
#include "int.h"
#include "main.h"

void uart_init(void);

#define KEY_DOWN		NUM_EINT2		// down
#define KEY_BACK		NUM_EINT16_31	// gongyong 


static void delay(int num)
{
	volatile int n = num;
	while (n--)
	{
		volatile int m = 10000;
		while (m--);
	}
}

int main(void)
{
	int n = 0;
	
	uart_init();
	
	// 如果程序中要使用中断，就要调用中断初始化来初步初始化中断控制器
	system_init_exception();
	
	eint_init();
	
	// 绑定isr到中断控制器硬件
	intc_setvectaddr(KEY_DOWN, key_isr_eint2);
	intc_setvectaddr(KEY_BACK, key_isr_eint16171819);
	intc_enable(KEY_DOWN);
	intc_enable(KEY_BACK);
	
	while (1)
	{
		printf("%d ", n++);
		delay(10000);
	}
	
	return 0;
}