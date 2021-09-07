#include <stdio.h>

int multip5(int a);
void multip5_2(void);
void func1(int *p);
void func2(const int *p);
void func3(char *p);
void func4(const char *p);



int x;		// 被乘5的变量，也就是输入函数的变量
int y;		// 输出结果的变量


int main(void)
{
	int a, b = 0, ret = -1;
	
	a = 30;
	ret = multip5_3(a, &b);
	if (ret == -1)
	{
		printf("出错了\n");
	}
	else
	{
		printf("result = %d.\n", b);
	}
	
/*
	char *pStr = "linux";				// 
	//char pStr[] = "linux";			// ok的
	func3(pStr);
	printf("%s.\n", pStr);
*/	
/*
	int a = 1;
	func1(&a);
	func2(&a);
*/
	
/*
	// 程序要完成功能是：对一个数乘以5
	// 第一种方法：函数传参
	int a = 3;
	int b;
	b = multip5(a);
	printf("result = %d.\n", b);
	
	// 第二种方法：用全局变量来传参
	x = 2;
	multip5_2();
	printf("y = %d.\n", y);
*/	
	
	
	return 0;
}

void multip5_2(void)
{
	y = 5 * x;
}


int multip5(int a)
{
	return a*5;
}

int multip5_3(int a, int *p)
{
	int tmp;

	tmp = 5 * a;
	if (tmp > 100)
	{
		return -1;
	}
	else
	{
		*p = tmp;
		return 0;
	}
}


void func1(int *p)
{
	*p = 5;
}
/*
void func2(const int *p)
{
	*p = 5;
}
*/

void func3(char *p)
{
	*p = 'a';
}

/*
void func4(const char *p)
{
	*p = 'a';
}
*/















