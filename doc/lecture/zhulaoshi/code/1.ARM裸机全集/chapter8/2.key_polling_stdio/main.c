#include "stdio.h"

void uart_init(void);

int main(void)
{
	uart_init();
	key_init();
	
	key_polling();
	
	return 0;
}