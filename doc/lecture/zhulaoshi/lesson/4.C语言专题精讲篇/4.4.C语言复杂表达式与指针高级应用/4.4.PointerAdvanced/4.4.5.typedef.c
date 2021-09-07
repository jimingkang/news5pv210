#include <stdio.h>

// 结构体类型的定义
/*
struct student
{
	char name[20];
	int age;
};
*/

// 定义了一个结构体类型，这个类型有2个名字：第一个名字是struct student，第二个类型名叫student_t
/*
typedef struct student
{
	char name[20];
	int age;
}student_t;
*/

// 第一个类型名：struct student，第二个类型名是student
typedef struct student
{
	char name[20];
	int age;
}student;


// 我们一次定义了2个类型：
// 第一个是结构体类型，有2个名字：struct teacher，teacher
// 第二个是结构体指针类型，有2个名字：struct teacher *， pTeacher
typedef struct teacher
{
	char name[20];
	int age;
	int mager;
}teacher, *pTeacher;

typedef int *PINT;
typedef const int *CPINT;

// const int *p和int *const p是不同的。前者是p指向的变量是const，后者是p本身const

int main(void)
{
	int a = 23;
	int b = 11;

	CPINT p = &a;
	*p = 33;				// error: assignment of read-only location ‘*p’
	p = &b;

/*	
	PINT const p = &a;
	
	*p = 33;
	p = &b;					// error: assignment of read-only variable ‘p’
*/
/*	
	PINT p1 = &a;
	
	const PINT p2 = &a;		// const int *p2;	或者 int *const p2;
	*p2 = 33;
	printf("*p2 = %d.\n", *p2);
	
	p2 = &b;				// error: assignment of read-only variable ‘p2’
*/	
	
	/*
	teacher t1;
	t1.age = 23;
	pTeacher p1 = &t1;
	printf("teacher age = %d.\n", p1->age);
	
	
	struct student *pS1;		// 结构体指针
	student *pS2;				// 同上
*/	
	/*
	struct student s1;			// struct student是类型；s1是变量
	s1.age = 12;
	
	student s2;
*/	
	
	return 0;
}






















