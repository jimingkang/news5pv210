#include <stdio.h>

int main(void)
{
	int a;
	int *p = NULL;
	// 中间省略400行代码······
	//p = (int *)4;			// 4地址不是你确定可以访问的，就不要用指针去解引用
	
	p = &a;			// 正确的使用指针的方式，是解引用指针前跟一个绝对可用的地址绑定

	//if (p != NULL)
	if (NULL != p)
	{
		*p = 4;
	}
	p = NULL;		// 使用完指针变量后，记得将其重新赋值为NULL
	
/*
	int *p;		// 局部变量，分配在栈上，栈反复被使用，所以值是随机的
	
	//printf("p = %p.\n",p);
	*p = 4;		// Segmentation fault (core dumped)运行时段错误，原因为野指针
*/
	
	return 0;
}



























