#include <stdio.h>

// 宏定义来解决返回值问题
#define FALSE 0
#define TRUE 1

// 这个枚举用来表示函数返回值，ERROR表示错，RIGHT表示对
enum return_value
{
	ERROR,				// 枚举值常量是全局的，直接自己就可以用。
	RIGHT,
};

enum return_value func1(void);


int main(void)
{
	enum return_value r = func1();

	if (r == RIGHT)			// 不是r.RIGHT，也不是return_value.RIGHT
	{
		printf("函数执行正确\n");
	}
	else
	{
		printf("函数执行错误\n");
	}
	
	printf("ERROR = %d.\n", ERROR);
	printf("RIGHT = %d.\n", RIGHT);
	
	
	return 0;
}



enum return_value func1(void)
{
	enum return_value r1;
	r1 = ERROR;
	return r1;
}

// 返回1表示执行成功，返回0表示执行失败
int func2(void)
{
	return 1;
}

int func3(void)
{
	return FALSE;
}












