#include "int.h"
#include "stdio.h"



void system_init_exception(void)
{
	r_exception_reset = (unsigned int)reset_exception;
	r_exception_undef = (unsigned int)undef_exception;
	r_exception_sotf_int = (unsigned int)sotf_int_exception;
	r_exception_prefetch = (unsigned int)prefetch_exception;
	r_exception_data = (unsigned int)data_exception;
	r_exception_irq = (unsigned int)IRQ_handle;
	r_exception_fiq = (unsigned int)IRQ_handle;
	

}

void reset_exception(void)
{
	printf("reset_exception.\n");
}

void undef_exception(void)
{
	printf("undef_exception.\n");
}

void sotf_int_exception(void)
{
	printf("sotf_int_exception.\n");
}

void prefetch_exception(void)
{
	printf("prefetch_exception.\n");
}

void data_exception(void)
{
	printf("data_exception.\n");
}



// 真正的中断处理程序。意思就是说这里只考虑中断处理，不考虑保护/恢复现场
void irq_handler(void)
{
	//printf("irq_handler.\n");
	// SoC支持很多个（在低端CPU例如2440中有30多个，在210中有100多个）中断
	// 这么多中断irq在第一个阶段走的是一条路，都会进入到irq_handler来
	// 我们在irq_handler中要去区分究竟是哪个中断发生了，然后再去调用该中断
	// 对应的isr。

}







