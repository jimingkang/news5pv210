#include <stdio.h>

// �ṹ�����͵Ķ���
/*
struct student
{
	char name[20];
	int age;
};
*/

// ������һ���ṹ�����ͣ����������2�����֣���һ��������struct student���ڶ�����������student_t
/*
typedef struct student
{
	char name[20];
	int age;
}student_t;
*/

// ��һ����������struct student���ڶ�����������student
typedef struct student
{
	char name[20];
	int age;
}student;


// ����һ�ζ�����2�����ͣ�
// ��һ���ǽṹ�����ͣ���2�����֣�struct teacher��teacher
// �ڶ����ǽṹ��ָ�����ͣ���2�����֣�struct teacher *�� pTeacher
typedef struct teacher
{
	char name[20];
	int age;
	int mager;
}teacher, *pTeacher;

typedef int *PINT;
typedef const int *CPINT;

// const int *p��int *const p�ǲ�ͬ�ġ�ǰ����pָ��ı�����const��������p����const

int main(void)
{
	int a = 23;
	int b = 11;

	CPINT p = &a;
	*p = 33;				// error: assignment of read-only location ��*p��
	p = &b;

/*	
	PINT const p = &a;
	
	*p = 33;
	p = &b;					// error: assignment of read-only variable ��p��
*/
/*	
	PINT p1 = &a;
	
	const PINT p2 = &a;		// const int *p2;	���� int *const p2;
	*p2 = 33;
	printf("*p2 = %d.\n", *p2);
	
	p2 = &b;				// error: assignment of read-only variable ��p2��
*/	
	
	/*
	teacher t1;
	t1.age = 23;
	pTeacher p1 = &t1;
	printf("teacher age = %d.\n", p1->age);
	
	
	struct student *pS1;		// �ṹ��ָ��
	student *pS2;				// ͬ��
*/	
	/*
	struct student s1;			// struct student�����ͣ�s1�Ǳ���
	s1.age = 12;
	
	student s2;
*/	
	
	return 0;
}






















