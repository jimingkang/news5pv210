#include "stdio.h"

void uart_init(void);

int mymain(void)
{
	uart_init();
	
	
	
	unsigned int *p1 = (unsigned int *)0x20000000;
	*p1 = 123;
	printf("*p1 = %d.\n", *p1);
	
	unsigned int *p2 = (unsigned int *)0x2F000000;
	*p2 = 123;
	printf("*p2 = %d.\n", *p2);
	
	unsigned int *p3 = (unsigned int *)0x30000000;
	*p3 = 123;
	printf("*p3 = %d.\n", *p3);
	
	unsigned int *p4 = (unsigned int *)0x3F000000;
	*p4 = 123;
	printf("*p4 = %d.\n", *p4);
	
	
	
	
	
	
	int a = 1234;
	putc('a');
	putc('b');
	putc('c');
	printf("test for printf, a = %d.\n", a);
	/*
	while(1)
	{
		uart_putc('a');
		delay();
	}
	*/
	return 0;
}