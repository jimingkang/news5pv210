#include <stdio.h>
#include <string.h>


#define dpChar char *
typedef char * tpChar;		// typedef用来重命名类型，或者说用来制造用户自定义类型


// func完全等同于func1
void func(int a[])
{
	printf("数组大小 = %d.\n", sizeof(a));
}

void func1(int *a)
{
	printf("数组大小 = %d.\n", sizeof(a));
}

void func2(int *a, int num)
{
	// 在子函数内，a是传进来的数组的指针（首地址）
	// 在子函数内，num是数组的大小
}



int main(void)
{
	int a[56];
	int b = sizeof(a) / sizeof(a[0]);	// 整个数组字节数/数组中一个元素的字节数
	printf("b = %d.\n", b);				// 结果应该是数组的元素个数
	
	
/*	
	dpChar p1,  p2;			// 展开：char *p1, p2; 相当于char *p1, char p2;
	tpChar p3,  p4;			// 等价于：char *p3, char *p4;	
	printf("sizeof(p1) = %d.\n", sizeof(p1));		// 4
	printf("sizeof(p2) = %d.\n", sizeof(p2));		// 1
	printf("sizeof(p3) = %d.\n", sizeof(p3));		// 4
	printf("sizeof(p4) = %d.\n", sizeof(p4));		// 4
*/
	
/*
	int a[20];
	func(a);			// 4 因为a在函数func内部就是指针，而不是数组
	
	func1(a);
	
	func2(a, sizeof(a));
*/
	
/*	
	int b1[100] = {0};
	printf("sizeof(b1) = %d.\n", sizeof(b1));		// 400 100×sizeof(int)
	
	short b2[100] = {};
	printf("sizeof(b2) = %d.\n", sizeof(b2));		// 200 100×sizeof(short)
	
	double b3[100];
	printf("sizeof(b3) = %d.\n", sizeof(b3));		// 800 100×sizeof(double)
*/	
	
/*
	int n = 10;
	printf("sizeof(n) = %d.\n", sizeof(n));			// 4
	printf("sizeof(int) = %d.\n", sizeof(int));		// 4
*/
	
/*
	char str[] = "hello";  
	char *p = str; 
	printf("sizeof(p) = %d.\n", sizeof(p));			// 4 相当于sizeof(char *)
	printf("sizeof(*p) = %d.\n", sizeof(*p));		// 1 相当于sizeof(char)
	printf("strlen(p) = %d.\n", strlen(p));			// 5 相当于strlen(str)
*/
	
/*
	char str[] = "hello";  
	printf("sizeof(str) = %d.\n", sizeof(str));				// 6
	printf("sizeof(str[0]) = %d.\n", sizeof(str[0]));		// 1
	printf("strlen(str) = %d.\n", strlen(str));				// 5
*/	
	
	return 0;
}




























