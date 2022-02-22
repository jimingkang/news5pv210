

void main(void)
{
char a='T';
	uart_init();
	
	while(1)
	{
a=uart_getc();
		uart_putc(a);
		delay();
	}
}