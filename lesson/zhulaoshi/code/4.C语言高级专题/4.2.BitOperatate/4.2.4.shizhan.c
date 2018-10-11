#include <stdio.h>


#define VALUE1	937
#define VALUE2	17

int main(void)
{
	
	a |= ((VALUE1<<7) | (VALUE2<<21));	
	
	/*
	// 第二种解法
	unsigned int a = 0xc30288f8;

	a &= ~((0x3ff<<7) | (0x1f<<21));	// bit7～bit17和bit21～bit25全清零
	a |= ((937<<7) | (17<<21));			// 937和17全部赋值

	printf("a = 0x%x.\n", a);		// 0xc223d4f8
	*/
	
	/*
	// 第一种解法：直接double第6题。
	unsigned int a = 0xc30288f8;
	// bit7～bit17赋值937
	a &= ~(0x3ff<<7);				// bit7~ bit17清零
	a |= 937<<7;
	// bit21～bit25赋值17
	a &= ~(0x1f<<21);				// bit21～bit25清零
	a |= 17<<21;					
	printf("a = 0x%x.\n", a);		// 0xc223d4f8
	*/
	
	/*
	unsigned int a =  0xc30288f8;		// 0xc34648f8
	//第一步，先读出原来bit7～bit17的值
	unsigned int tmp = 0;
	tmp = a & (0x3ff<<7);
	//printf("befor shift, tmp = 0x%x.\n", tmp);
	tmp >>= 7;			// 鬼在这里。
	//printf("after shift, tmp = 0x%x.\n", tmp);
	//第二步，给这个值加17
	tmp += 0;
	
	//第三步，将a的bit7～bit17清零
	a &= ~(0x3ff<<7);
	//第四步，将第二步算出来的值写入bit7～bit17
	a |= tmp<<7;
	printf("a = 0x%x.\n", a);
	*/
	
	/*
	unsigned int a =  0xc30288f8;
	// 第一步，bit7～bit17清零
	a &= ~(0x7ff<<7);
	// 第二步，bit7～bit17赋值937
	a |= (937<<7);
	printf("a = 0x%x.\n", a);
	*/
	
	/*
	unsigned int a =  0xc30288f8;
	// 第一步，把bit3到bit8的位保持不变，其余位全部清零
	a &= (0x3f<<3);
	// 第二步，将之右移3位
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
	a |= (0b11111<<3);		// bit3开始，连续5位，也就是bit3～bit7
	
	printf("a = 0x%x.\n", a);
	*/
	
	
	return 0;
}

























