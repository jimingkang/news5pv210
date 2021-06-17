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



// �������жϴ��������˼����˵����ֻ�����жϴ��������Ǳ���/�ָ��ֳ�
void irq_handler(void)
{
	//printf("irq_handler.\n");
	// SoC֧�ֺܶ�����ڵͶ�CPU����2440����30�������210����100������ж�
	// ��ô���ж�irq�ڵ�һ���׶��ߵ���һ��·��������뵽irq_handler��
	// ������irq_handler��Ҫȥ���־������ĸ��жϷ����ˣ�Ȼ����ȥ���ø��ж�
	// ��Ӧ��isr��

}







