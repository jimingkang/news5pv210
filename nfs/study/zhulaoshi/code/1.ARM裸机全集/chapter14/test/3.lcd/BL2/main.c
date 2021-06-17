static void delay(void)
{
	volatile unsigned int i, j;
	for (i=0; i<4000; i++)
	{
		for (j=0; j<1000; j++);
	}
}

int main(void)
{
	lcd_test();
	
	while (1);
	
	return 0;
}