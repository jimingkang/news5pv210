#include <stdio.h>

int main(void)
{
	const int a = 5;
	//a = 6;				// error: assignment of read-only variable ��a��
	int *p;
	p = (int *)&a;			// ���ﱨ���߿���ͨ��ǿ������ת��������
	*p = 6;
	printf("a = %d.\n", a);	// a = 6,���֤��const���͵ı���������
	
	
	
/*
	int a = 5;
	
	// ��һ��
	const int *p1;		// p������cosnt�ģ���pָ��ı�����const��
	// �ڶ���
	int const *p2;		// p������cosnt�ģ���pָ��ı�����const��
	
	
	// ������
	int * const p3;		// p������cosnt�ģ�pָ��ı�������const��
	// ������
	const int * const p4;// p������cosnt�ģ�pָ��ı���Ҳ��const��
	
	*p1  = 3;		// error: assignment of read-only location ��*p1��
	p1 = &a;		// �����޴����޾���
	
	*p2 = 5;		// error: assignment of read-only location ��*p2��
	p2 = &a;		// �����޴����޾���
	
	*p3 = 5;		// �����޴����޾���
	p3 = &a;		// error: assignment of read-only variable ��p3��
	
	p4 = &a;		// error: assignment of read-only variable ��p4��
	*p4 = 5;		// error: assignment of read-only location ��*p4��
*/	
	
	return 0;
}














