#include <stdio.h>

int a = 111;
extern int g_a;


void func(void)
{
	int a = 5;	// 作用域范围是8-11行
	
	printf("in func, a = %d.\n", a);
}


int main(void)
{
	//printf("in main, g_a = %d.\n", g_a);
	
	// 在20-23行范围内，44这个a把111那个a给掩蔽掉了
	int a;			// a的作用域范围就是19行到22行
	a = 44;
	printf("in main, a = %d.\n", a);
}

int g_a = 111;			// 作用域是4行之后

























