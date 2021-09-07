#include <stdio.h>


int main(void)
{
	// 强制类型转换
	int a;
	char b;
	a = 1;
	b = (char)a;
	printf("b = %d.\n", b);
	
/*
	// 移位
	int a, b;
	a = 1;
	b = a >> 1;
	printf("b = %d.\n", b);
	*/
/*
	// 位与
	int a = 1;
	int b = a & 0xff;		// 也可以写成：char b = a & 0x01;
	printf("b = %d.\n", b);
*/

	
	return 0;
}


















