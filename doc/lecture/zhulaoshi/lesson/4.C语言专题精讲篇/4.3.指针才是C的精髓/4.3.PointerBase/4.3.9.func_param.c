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
	char a;				// �ṹ�������������
	int b;				// ��ΪҪ�����ţ����Դ�С��8
};

void func5(struct A *a1)
{
	printf("sizeof(a1) = %d.\n", sizeof(a1));		// 4
	printf("sizeof(*a1) = %d.\n", sizeof(*a1));		// 8
	printf("&a1 = %p.\n", &a1);			// ����ָ��
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


// &a��&b��ͬ��˵��a��b����ͬһ�����������ڴ���a��b�Ƕ�����2���ڴ�ռ䣩
// ����a��b���й����ģ�ʵ����b��a��ֵ�õ��ġ�
void func1(int b)
{
	// �ں����ڲ����β�b��ֵ����ʵ��a
	printf("b = %d.\n", b);
	printf("in func1, &b = %p.\n", &b);
}

int main(void)
{
	int x = 3, y = 5;
	swap2(&x, &y);
	printf("x = %d, y = %d.\n", x, y);		// �����ɹ�
	
/*
	int x = 3, y = 5;
	swap1(x, y);
	printf("x = %d, y = %d.\n", x, y);		// x=3,y=5������ʧ��
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




































