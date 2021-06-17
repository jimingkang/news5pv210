#include <stdio.h>


// 共用体类型的定义
struct mystruct
{
	int a;
	char b;
};

// a和b其实指向同一块内存空间，只是对这块内存空间的2种不同的解析方式。
// 如果我们使用u1.a那么就按照int类型来解析这个内存空间；如果我们使用u1.b那么就按照char类型
// 来解析这块内存空间。
union myunion
{
	int a;
	char b;
};

typedef union xx
{
	int a;
	char b;
	double c;
}XXX;

union test
{
	int a;
	float b;
};

int main(void)
{
	union test t1;
	t1.a = 1123477881;
	printf("value = %f.\n", t1.b);
	
	int a = 1123477881;
	printf("指针方式：%f.\n", *((float *)&a));
	
	
/*
	// test1
	struct mystruct s1;
	s1.a = 23;
	printf("s1.b = %d.\n", s1.b);		// s1.b = 0. 结论是s1.a和s1.b是独立无关的
	printf("&s1.a = %p.\n", &s1.a);
	printf("&s1.b = %p.\n", &s1.b);
	
	union myunion u1;		// 共用体变量的定义
	u1.a = 23;				// 共用体元素的使用
	printf("u1.b = %d.\n", u1.b);		// u1.b = 23.结论是u1.a和u1.b是相关的
	// a和b的地址一样，充分说明a和b指向同一块内存，只是对这块内存的不同解析规则
	printf("&u1.a = %p.\n", &u1.a);
	printf("&u1.b = %p.\n", &u1.b);
	
	printf("sizeof(XXX) = %d.\n", sizeof(XXX));
	printf("sizeof(union xx) = %d.\n", sizeof(union xx));
*/	
	return 0;
}



















