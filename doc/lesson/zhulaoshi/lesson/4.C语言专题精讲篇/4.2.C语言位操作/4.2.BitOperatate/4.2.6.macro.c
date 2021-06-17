#include <stdio.h>


// ��һ�⣺�ú궨�彫32λ��x�ĵ�nλ���ұ����㣬Ҳ����bit0���1λ����λ
#define SET_BIT_N(x, n)			(x | (1U<<(n-1)))

// �ڶ��⣺�ú궨�彫32λ��x�ĵ�nλ���ұ����㣬Ҳ����bit0���1λ������
#define CLEAR_BIT_N(x, n)		(x & ~(1U<<(n-1)))

// �����⣺�ú궨�彫32λ��x�ĵ�nλ����mλ���ұ����㣬Ҳ����bit0���1λ��m�Ǹ�λ����λ
// ����������n=3��m=6����Ŀ����Ҫ��bit2��bit5��λ
// ������Ҫһ����ʽ���õ�(m-n+1)��1
// �㷨����1�����ȵõ�32λ1��	~0U
//       ��2��������1���õ���������xλ���ɵõ�(m-n+1)��1	(~0U)>>(32-(m-n+1))
#define SET_BIT_N_M(x, n, m)	(x | (((~0U)>>(32-(m-n+1)))<<(n-1)))

// �����⣺��ȡ����x�ĵ�n����mλ



int main(void)
{
	// д������� ~�� << ˭�����ȼ���
	unsigned int a = 0xf;
	unsigned int b = 0;
	b = ~a<<4;					// �����~����ǣ�0xFFFFFF00	�����<<����ǣ�0xffffff0f
	printf("b = 0x%x.\n", b);	// �����0xffffff00��˵��~���ȼ���
	
	
	/*
	unsigned int a = 0x0;
	unsigned int b = 0;
	b = SET_BIT_N_M(a, 5, 8);
	printf("b = 0x%x.\n", b);
	*/
	
	/*
	// test for CLEAR_BIT_N
	unsigned int a = 0xFFFFFFFF;
	unsigned int b = 0;
	b = CLEAR_BIT_N(a, 4);
	printf("b = 0x%x.\n", b);
	*/
	
	/*
	// test for SET_BIT_N
	unsigned int a = 0;
	unsigned int b = 0;
	b = SET_BIT_N(a, 4);
	printf("b = 0x%x.\n", b);
	*/
	
	return 0;
}

















