#include <stdio.h>


// �������ܷ��غ����ڲ��ֲ������ĵ�ַ����Ϊ�������ִ���귵�غ�����ֲ������Ѿ�������
// ����ֲ������Ƿ�����ջ�ϵģ���Ȼ�����˵���ջ�ڴ滹�ڻ����Է��ʣ����Ƿ���ʱʵ�������
// �ڴ��ַ�Ѿ��͵�ʱ�Ǹ������޹��ˡ�
int *func(void)
{
	int a = 4;			// a�Ǿֲ�������������ջ���ֽ�ջ�������ֽ���ʱ����
	printf("&a = %p\n", &a);
	return &a;
}

void func2(void)
{
	int a = 33;
	int b = 33;
	int c = 33;
	printf("in func2, &a = %p\n", &a);
}

void stack_overflow(void)
{
	int a[10000000] = {0};
	a[10000000-1] = 12;
}

void stack_overflow2(void)
{
	int a = 2;
	stack_overflow2();
}


int main(void)
{
	//stack_overflow();
	stack_overflow2();
/*
	int *p = NULL;
	p = func();
	func2();
	func2();
	printf("p = %p\n", p);
	
	printf("*p = %d.\n", *p); 		// ֤��ջ�ڴ����˺������
*/	
	return 0;
}

















