#include <stdio.h>

// 定义类型
struct people
{
	char name[20];
	int age;
};

// 定义类型的同时定义变量。
struct student
{
	char name[20];
	int age;
}s1;

/*
// 将类型struct student重命名为s1，s1是一个类型名，不是变量
typedef struct student
{
	char name[20];
	int age;
}s1;
*/

struct score
{
	int a;
	int b;
	int c;
};

struct myStruct
{
	int a;			// 4 
	double b;		// 8
	char c;
};

struct s
{
	char c;			// 1  c实际占4字节，而不是1字节
	int b;			// 4
};


int main(void)
{
	printf("sizeof(struct s) = %d.\n", sizeof(struct s));	//  5 or 8? 结果是8
	
/*
	struct s s1;
	s1.c = 't';
	s1.b = 12;
	
	char *p1 = (char *)(&s1);
	printf("*p1 = %c.\n", *p1);			// t
	
	int *p2 = (int *)((int)&s1 + 1);		
	printf("*p2 = %d.\n", *p2);			// 201852036.
	
	int *p3 = (int *)((int)&s1 + 4);		
	printf("*p3 = %d.\n", *p3);			// 12.
*/

	
/*
	struct myStruct s1;
	s1.a = 12;		// int *p = (int *)&s1; *p = 12;
	s1.b = 4.4;		// double *p = (double *)(&s1 + 4); *p = 4.4;
	s1.c = 'a';		// char *p = (char *)((int)&s1 + 12); *p = 'a';
*/

/*	
	printf("s1.a = %d.\n", s1.a);

	int *p = (int *)&s1; 
	printf("*p = %d.\n", *p);
*/

/*	
	printf("s1.b = %lf.\n", s1.b);
	
	double *p = (double *)((int)&s1 + 4); 
	printf("*p = %lf.\n", *p);
*/
/*
	printf("s1.c = %c.\n", s1.c);

	char *p = (char *)((int)&s1 + 12);
	printf("*p = %c.\n", *p);
*/
	
/*
	int a[3];		// 3个学生的成绩，数组方式
	score s;		// 3个学生的成绩，结构体的方式
	
	s.a = 12;		// 编译器在内部还是转成指针式访问 int *p = s; *(p+0) = 12;
	s.b = 44;		// int *p = s; *(p+1) = 44;
	s.c = 64;		// int *p = s; *(p+2) = 44;
*/	
	
	
/*	
	struct people p1;		// 使用结构体类型定义变量
	s1.age = 23;
	printf("s1.age = %d.\n", s1.age);
	
	struct student s2;
	s2.age = 11;
	printf("s2.age = %d.\n", s2.age);
*/	
	return 0;
}












