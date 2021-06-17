#include <stdio.h>

//extern int g_a;
extern int g_b;

int g_a = 3;				// 定义兼且声明，定义本身就有声明，但是声明没有定义
							// 思考：为什么全局变量都要定义在c文件头部而不是尾部
int main(void)
{
	volatile int a, b, c;
	a = 3;
	b = a;
	c = b;			// 等效于 c = b = a = 3;
	// 无优化情况下：内存要读取3次，写3次
	// 编译器优化为：内存只要读1次，写3次
	
	
	
	/*
	g_b = 33;
	printf("g_b = %d.\n", g_b);
	*/

	return 0;
}









