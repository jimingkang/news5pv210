

void main(void)
{
	uart_init();
	
	while(1)
	{
		uart_putc('a');
		delay();
	}
}