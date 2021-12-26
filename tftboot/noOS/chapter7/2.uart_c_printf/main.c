#include "stdio.h"

void uart_init(void);

int main(void)
{
	uart_init();
	
	char a = 'T';
	putc('a');
	putc('b');
	putc('c');
	while (1)
	{a=getc();
	printf("test for printf, a = %d.\r\n", a);
	}
	
	/*
	while(1)
	{
		uart_putc('a');
		delay();
	}
	*/
	return 0;
}