#include <stdio.h>

int main(void)
{
	/*
	 *	45:101101
	 *	23:010111
	 *	58:111010		^����
	 */
	
	// λ���
	unsigned int a = 45, b = 23;
	unsigned int c;
	c = a ^ b;					// 58
	printf("c = %d.\n", c);
	
	/*
	// ��λ�Ͱ��߼�ȡ����ȡ��
	unsigned int a = 45;
	unsigned int b, c;
	b = ~~a;						// ��λȡ�������λ������1��0��0��1
	c = !!a;						// ���߼�ȡ�������٣��ٱ���
	printf("b = %u.\n", b);
	printf("c = %d.\n", c);
	*/
	
	
	/*
	// ��λ�Ͱ��߼�ȡ��
	unsigned int a = 45;
	unsigned int b, c;
	b = ~a;						// ��λȡ�������λ������1��0��0��1
	c = !a;						// ���߼�ȡ�������٣��ٱ���
	printf("b = %u.\n", b);
	printf("c = %d.\n", c);
	*/
	
	return 0;
}
