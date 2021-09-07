#include <stdio.h>


void func(int **p)
{
	*p = (int *)0x12345678;
}



int main(void)
{
	int a = 4;
	int *p = &a;				// p指向a
	printf("p = %p.\n", p);		// p打印出来就是a的内存地址
	func(&p);					// 在func内部将p指向了别的地方
	printf("p = %p.\n", p);		// p已经不指向a了，所以打印出来不是a的地址
	*p = 23;					// 因为此时p指向0x12345678，但是这个地址是不
								// 允许访问的，因此会段错误。
	
/*	
	int *p1[5];
	int *p2;
	int **p3;
	
	//p2 = p1;
	p3 = p1;		// p1是指针数组名，本质上是数组名，数组名做右值表示数组首元素
					// 首地址。数组的元素就是int *类型，所以p1做右值就表示一个int *
					// 类型变量的地址，所以p1就是一个int类型变量的指针的指针，所以
					// 它就是一个二重指针int **；
*/	
	
/*
	char a;
	
	char **p1;		// 二重指针
	char *p2;		// 一重指针
	
	printf("sizeof(p1) = %d.\n", sizeof(p1));
	printf("sizeof(p2) = %d.\n", sizeof(p2));
	
	p2 = &a;
	//p1 = &a;		// p1是char **类型，&a是char *类型。
					// char **类型就是指针指向的变量是char *类型
					// char *类型表示指针指向的变量是char类型。
	p1 = &p2;		// p2本身是char *类型，再取地址变成char **类型，和p1兼容。
*/	
	
	return 0;
}



























