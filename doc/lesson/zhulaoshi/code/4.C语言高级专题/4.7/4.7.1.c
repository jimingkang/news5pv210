#include <stdio.h>

int var = 10;

int main(void)
{
	printf("in mian£¬ var = %d.\n", var);		// var = 10;
	int var = 5;
	
	if (1)
	{
		int var = 2;
		printf("in if, var = %d.\n", var);
	}
	
	printf("var = %d.\n", var);
	
/*
	int i;
	for (i=0; i<10; i++)
	{
		int a = 5;
		printf("i = %d.\n", i);
	}
	a++;			// error: ¡®a¡¯ undeclared
*/	
	return 0;
}

























