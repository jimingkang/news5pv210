#include "aston.h"
#include <stdio.h>


void func1(void)
{
	printf("func1\n");
}

int func2(int a, int b)
{
	printf("func2, a = %d, b = %d.\n", a, b);
	return a+b;
}