#include <stdio.h>

int main(void)
{
	int *p = (int *)malloc(4);		// 由void *强制转为int *，不会警告
	
	/*
	void a;
	a = 5;
	*/
	
	/*
	void a;
	int b = (int)a;
	b = 23;
	*/
	
	return 0;
}















