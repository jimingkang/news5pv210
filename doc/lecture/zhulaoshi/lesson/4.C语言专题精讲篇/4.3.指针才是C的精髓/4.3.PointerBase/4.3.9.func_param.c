#include <stdio.h>

void swap1(int a, int b)
{
	int tmp;
	tmp = a;
	a = b;
	b = tmp;
	printf("in swap1, a = %d, b = %d.\n", a, b);
}

void swap2(int *a, int *b)
{
	int tmp;
	tmp = *a;
	*a = *b;
	*b = tmp;
	printf("in swap1, *a = %d, *b = %d.\n", *a, *b);
}




struct A
{
	char a;				// 结构体变量对齐问题
	int b;				// 因为要对齐存放，所以大小是8
};

void func5(struct A *a1)
{
	printf("sizeof(a1) = %d.\n", sizeof(a1));		// 4
	printf("sizeof(*a1) = %d.\n", sizeof(*a1));		// 8
	printf("&a1 = %p.\n", &a1);			// 二重指针
	printf("a1 = %p.\n", a1);
	printf("a1->b = %d.\n", a1->b);
}

void func4(struct A a1)
{
	printf("sizeof(a1) = %d.\n", sizeof(a1));
	printf("&a1 = %p.\n", &a1);
	printf("a1.b = %d.\n", a1.b);
}

void func3(int *a)
{
	printf("sizeof(a) = %d.\n", sizeof(a));
	printf("in func2, a = %p.\n", a);
}

void func2(int a[])
{
	printf("sizeof(a) = %d.\n", sizeof(a));
	printf("in func2, a = %p.\n", a);
}


// &a和&b不同，说明a和b不是同一个变量（在内存中a和b是独立的2个内存空间）
// 但是a和b是有关联的，实际上b是a赋值得到的。
void func1(int b)
{
	// 在函数内部，形参b的值等于实参a
	printf("b = %d.\n", b);
	printf("in func1, &b = %p.\n", &b);
}

int main(void)
{
	int x = 3, y = 5;
	swap2(&x, &y);
	printf("x = %d, y = %d.\n", x, y);		// 交换成功
	
/*
	int x = 3, y = 5;
	swap1(x, y);
	printf("x = %d, y = %d.\n", x, y);		// x=3,y=5，交换失败
*/
	
/*
	struct A a = 
	{
		.a = 4,
		.b = 5555,
	};
	printf("sizeof(a) = %d.\n", sizeof(a));		// 4
	//printf("sizeof(*a) = %d.\n", sizeof(*a));		// 8
	printf("&a = %p.\n", &a);
	//printf("a = %p.\n", a);
	printf("a.b = %d.\n", a.b);
	func5(&a);
*/
	
/*
	struct A a = 
	{
		.a = 4,
		.b = 5555,
	};
	printf("sizeof(a) = %d.\n", sizeof(a));
	printf("&a = %p.\n", &a);
	printf("a.b = %d.\n", a.b);
	func4(a);
*/
	
/*	
	int a[5];
	printf("a = %p.\n", a);
	func3(a);
*/
	
/*	
	int a[5];
	printf("a = %p.\n", a);
	func2(a);
*/	
	
/*	
	int a = 4;
	printf("&a = %p.\n", &a);
	func1(a);
*/	
	
	
	return 0;
}




































