#include <stdio.h>

int main(void)
{
	const int a = 5;
	//a = 6;				// error: assignment of read-only variable ‘a’
	int *p;
	p = (int *)&a;			// 这里报警高可以通过强制类型转换来消除
	*p = 6;
	printf("a = %d.\n", a);	// a = 6,结果证明const类型的变量被改了
	
	
	
/*
	int a = 5;
	
	// 第一种
	const int *p1;		// p本身不是cosnt的，而p指向的变量是const的
	// 第二种
	int const *p2;		// p本身不是cosnt的，而p指向的变量是const的
	
	
	// 第三种
	int * const p3;		// p本身是cosnt的，p指向的变量不是const的
	// 第四种
	const int * const p4;// p本身是cosnt的，p指向的变量也是const的
	
	*p1  = 3;		// error: assignment of read-only location ‘*p1’
	p1 = &a;		// 编译无错误无警告
	
	*p2 = 5;		// error: assignment of read-only location ‘*p2’
	p2 = &a;		// 编译无错误无警告
	
	*p3 = 5;		// 编译无错误无警告
	p3 = &a;		// error: assignment of read-only variable ‘p3’
	
	p4 = &a;		// error: assignment of read-only variable ‘p4’
	*p4 = 5;		// error: assignment of read-only location ‘*p4’
*/	
	
	return 0;
}














