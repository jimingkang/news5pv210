#include "stdio.h"

void uart_init(void);

#define KEY_INTNUM		NUM_EINT9		// 暂时随便找的，下节课会具体改


void key_isr(void)
{
	// 暂时随便写的，下节课正式的会写到key.c中去
}

int main(void)
{
	uart_init();
	key_init();
	
	// 如果程序中要使用中断，就要调用中断初始化来初步初始化中断控制器
	system_init_exception();
	
	// 绑定isr到中断控制器硬件
	intc_setvectaddr(KEY_INTNUM, key_isr);
	
	return 0;
}