#include <stdio.h>

int add(int a, int b);					// ��������	

int main(void)
{
	int a = 3, b = 5;
	int sum = add(a, b);				// ���͵ĺ�������
	printf("3+5=%d.\n", add(3, 5));		// add�����ķ���ֵ��Ϊprintf������һ������
	
	return 0;
}

// ��������
int add(int a, int b)		// �������������б�����ֵ
{
	return a + b;			//������
}























