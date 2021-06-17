// 用递归函数来计算阶乘
#include <stdio.h>

int jiecheng(int n);		// 函数声明
void digui(int n);

int main(void)
{
	digui(300);

	//int a = 5;
	//printf("%d的阶乘是：%d.\n", a, jiecheng(a));

	return 0;
}

// 函数定义
int jiecheng(int n)
{
	// 传参错误校验
	if (n < 1)
	{
		printf("n必须大于等于1.\n");
		return -1;
	}
	
	if (n == 1)
	{
		return 1;
	}
	else
	{
		return (n * jiecheng(n-1));
	}
}

void digui(int n)
{
	int a[100];
	//printf("递归前：n = %d.\n", n);
	if (n > 1)
	{
		digui(n-1);
	}
	else
	{
		printf("结束递归，n = %d.\n", n);
	}
	printf("递归后：n = %d.\n", n);
}








