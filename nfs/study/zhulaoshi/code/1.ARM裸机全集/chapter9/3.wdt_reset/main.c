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
	static int i = 0;
	uart_init();
	//key_init();
	wdt_init_reset();

	printf("---wdt interrupt test---, i = %d.", i++);
	

	while (1);

	return 0;
}