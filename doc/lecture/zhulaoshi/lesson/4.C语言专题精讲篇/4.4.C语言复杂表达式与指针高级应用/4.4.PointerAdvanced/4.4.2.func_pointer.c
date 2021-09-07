#include <stdio.h>
#include <string.h>

void func1(void)
{
	printf("I am func1.\n");
}

// 这句重命名了一种类型，这个新类型名字叫pType，类型是：char* (*)(char *, const char *);
typedef char* (*pType)(char *, const char *);

// 函数指针数组
typedef char* (*pType[5])(char *, const char *);
// 函数指针数组指针
typedef char* (*(*pType)[5])(char *, const char *);


int main(void)
{
	char* (*p1)(char *, const char *);
	char* (*p2)(char *, const char *);
	
	pType p3;		// 等效于 char* (*p3)(char *, const char *);
	pType p4;
	
	p3 = p1;
	
/*
	char a[5] = {0};
	char* (*pFunc)(char *, const char *);
	pFunc = strcpy;
	pFunc(a, "abc");
	printf("a = %s.\n", a);
*/

	
/*	
	void (*pFunc)(void);
	//pFunc = func1;			// 左边是一个函数指针变量，右边是一个函数名
	pFunc = &func1;				// &func1和func1做右值时是一模一样的，没任何区别
	pFunc();				// 用函数指针来解引用以调用该函数
*/
	
/*	
	int *p;
	int a[5];
	
	p = a;		// 般配的，类型匹配的，所以编译器不会警告不会报错。
	//p = &a;		// 类型不匹配，p是int *, &a是int (*)[5];
	
	int (*p1)[5] ;
	p1 = &a;		// p1类型是int (*)[5]，&a的类型也是int (*)[5]
*/	
	return 0;
}



















