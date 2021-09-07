#include "cal.h"
#include <stdio.h>


int add(int a, int b)
{
	return a + b;
}

int sub(int a, int b)
{
	return a - b;
}

int multiply(int a, int b)
{
	return a * b;
}

int divide(int a, int b)
{
	return a / b;
}


int main(void)
{
	int ret = 0;
	struct cal_t myCal;
	
	myCal.a = 12;
	myCal.b = 4;
	myCal.p = divide;
	
	ret = calculator(&myCal);
	printf("ret = %d.\n", ret);
	
	return 0;
}























