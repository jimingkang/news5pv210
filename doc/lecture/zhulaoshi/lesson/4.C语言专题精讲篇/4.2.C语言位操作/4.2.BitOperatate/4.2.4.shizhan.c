#include <stdio.h>


#define VALUE1	937
#define VALUE2	17

int main(void)
{
	
	a |= ((VALUE1<<7) | (VALUE2<<21));	
	
	/*
	// �ڶ��ֽⷨ
	unsigned int a = 0xc30288f8;

	a &= ~((0x3ff<<7) | (0x1f<<21));	// bit7��bit17��bit21��bit25ȫ����
	a |= ((937<<7) | (17<<21));			// 937��17ȫ����ֵ

	printf("a = 0x%x.\n", a);		// 0xc223d4f8
	*/
	
	/*
	// ��һ�ֽⷨ��ֱ��double��6�⡣
	unsigned int a = 0xc30288f8;
	// bit7��bit17��ֵ937
	a &= ~(0x3ff<<7);				// bit7~ bit17����
	a |= 937<<7;
	// bit21��bit25��ֵ17
	a &= ~(0x1f<<21);				// bit21��bit25����
	a |= 17<<21;					
	printf("a = 0x%x.\n", a);		// 0xc223d4f8
	*/
	
	/*
	unsigned int a =  0xc30288f8;		// 0xc34648f8
	//��һ�����ȶ���ԭ��bit7��bit17��ֵ
	unsigned int tmp = 0;
	tmp = a & (0x3ff<<7);
	//printf("befor shift, tmp = 0x%x.\n", tmp);
	tmp >>= 7;			// �������
	//printf("after shift, tmp = 0x%x.\n", tmp);
	//�ڶ����������ֵ��17
	tmp += 0;
	
	//����������a��bit7��bit17����
	a &= ~(0x3ff<<7);
	//���Ĳ������ڶ����������ֵд��bit7��bit17
	a |= tmp<<7;
	printf("a = 0x%x.\n", a);
	*/
	
	/*
	unsigned int a =  0xc30288f8;
	// ��һ����bit7��bit17����
	a &= ~(0x7ff<<7);
	// �ڶ�����bit7��bit17��ֵ937
	a |= (937<<7);
	printf("a = 0x%x.\n", a);
	*/
	
	/*
	unsigned int a =  0xc30288f8;
	// ��һ������bit3��bit8��λ���ֲ��䣬����λȫ������
	a &= (0x3f<<3);
	// �ڶ�������֮����3λ
	printf("a = 0x%x.\n", a);
	a >>= 3;
	printf("a = %u.\n", a);
	*/
	
	/*
	unsigned int a;
	a = 0xFFFFFFFF;
	a &= (~(0x1ff<<15));
	printf("a = 0x%x.\n", a);
	*/
	
	/*
	unsigned int a;
	a = 0xFFFFFFFF;
	a &= (~(1<<15));
	printf("a = 0x%x.\n", a);
	*/
	
	/*
	unsigned int a;
	
	a = 0;
	a |= (0b11111<<3);		// bit3��ʼ������5λ��Ҳ����bit3��bit7
	
	printf("a = 0x%x.\n", a);
	*/
	
	
	return 0;
}

























