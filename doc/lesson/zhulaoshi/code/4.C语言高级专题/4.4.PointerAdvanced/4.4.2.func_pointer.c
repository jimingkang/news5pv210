#include <stdio.h>
#include <string.h>

void func1(void)
{
	printf("I am func1.\n");
}

// �����������һ�����ͣ�������������ֽ�pType�������ǣ�char* (*)(char *, const char *);
typedef char* (*pType)(char *, const char *);

// ����ָ������
typedef char* (*pType[5])(char *, const char *);
// ����ָ������ָ��
typedef char* (*(*pType)[5])(char *, const char *);


int main(void)
{
	char* (*p1)(char *, const char *);
	char* (*p2)(char *, const char *);
	
	pType p3;		// ��Ч�� char* (*p3)(char *, const char *);
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
	//pFunc = func1;			// �����һ������ָ��������ұ���һ��������
	pFunc = &func1;				// &func1��func1����ֵʱ��һģһ���ģ�û�κ�����
	pFunc();				// �ú���ָ�����������Ե��øú���
*/
	
/*	
	int *p;
	int a[5];
	
	p = a;		// ����ģ�����ƥ��ģ����Ա��������ᾯ�治�ᱨ��
	//p = &a;		// ���Ͳ�ƥ�䣬p��int *, &a��int (*)[5];
	
	int (*p1)[5] ;
	p1 = &a;		// p1������int (*)[5]��&a������Ҳ��int (*)[5]
*/	
	return 0;
}



















