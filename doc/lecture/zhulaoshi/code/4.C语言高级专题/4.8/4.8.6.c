#include <stdio.h>


int main(void)
{
	int b;
	float a;
	b = 10;
	a = b / 3;		// 第一步先算 b/3，第二步将第一步的结果强制类型转换为float生成一个临时变量，第三步将第二步生成的临时变量赋值给a，第四步销毁临时变量。
	printf("a = %f.\n", a);
	
	/*
	int b;
	float a, f;
	b = 10;
	f = 1.5;
	a = b / f;
	printf("a = %f.\n", a);
	*/
	
	
	/*
	float a = 12.34;
	int b = (int)a;			// a被强制类型转换后自己本身竟然没变
	// (int)a强制类型转换并赋值在底层实际分了4个步骤：第一步先在另外的地方找一个内存构建一个临时变量x（x的类型是int，x的值等于a的整数部分），第二步将float a的值的整数部分赋值给x，第三步将x赋值给b，第四步销毁x。
	// 最后结果：a还是float而且值保持不变，b是a的整数部分。
	
	printf("a = %f, b = %d.\n", a, b);
	*/
	
	return 0;
}