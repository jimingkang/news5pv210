#include <stdio.h>

int add(int a, int b);					// 函数声明	

int main(void)
{
	int a = 3, b = 5;
	int sum = add(a, b);				// 典型的函数调用
	printf("3+5=%d.\n", add(3, 5));		// add函数的返回值作为printf函数的一个参数
	
	return 0;
}

// 函数定义
int add(int a, int b)		// 函数名、参数列表、返回值
{
	return a + b;			//函数体
}























