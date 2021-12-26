#include <stdio.h>

void uart_init(void);

int main(void)
{
	uart_init();
	
	putc('a');
	putc('b');
	putc('c');
	
	int a = 1234;
	while (1)
	{
		printf("printf %d.\n", a);
	}
	
}