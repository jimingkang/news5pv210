#include <stdio.h>

//extern int g_a;
extern int g_b;

int g_a = 3;				// ����������������屾�������������������û�ж���
							// ˼����Ϊʲôȫ�ֱ�����Ҫ������c�ļ�ͷ��������β��
int main(void)
{
	volatile int a, b, c;
	a = 3;
	b = a;
	c = b;			// ��Ч�� c = b = a = 3;
	// ���Ż�����£��ڴ�Ҫ��ȡ3�Σ�д3��
	// �������Ż�Ϊ���ڴ�ֻҪ��1�Σ�д3��
	
	
	
	/*
	g_b = 33;
	printf("g_b = %d.\n", g_b);
	*/

	return 0;
}









