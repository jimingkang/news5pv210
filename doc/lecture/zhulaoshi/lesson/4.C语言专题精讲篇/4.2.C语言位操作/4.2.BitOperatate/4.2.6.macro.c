#include <stdio.h>


// 第一题：用宏定义将32位数x的第n位（右边起算，也就是bit0算第1位）置位
#define SET_BIT_N(x, n)			(x | (1U<<(n-1)))

// 第二题：用宏定义将32位数x的第n位（右边起算，也就是bit0算第1位）清零
#define CLEAR_BIT_N(x, n)		(x & ~(1U<<(n-1)))

// 第三题：用宏定义将32位数x的第n位到第m位（右边起算，也就是bit0算第1位，m是高位）置位
// 分析：加入n=3，m=6，题目就是要把bit2到bit5置位
// 我们需要一个算式来得到(m-n+1)个1
// 算法：第1步：先得到32位1：	~0U
//       第2步：将第1步得到的数右移x位即可得到(m-n+1)个1	(~0U)>>(32-(m-n+1))
#define SET_BIT_N_M(x, n, m)	(x | (((~0U)>>(32-(m-n+1)))<<(n-1)))

// 第四题：截取变量x的第n到第m位



int main(void)
{
	// 写代码测试 ~和 << 谁的优先级高
	unsigned int a = 0xf;
	unsigned int b = 0;
	b = ~a<<4;					// 如果先~结果是：0xFFFFFF00	如果先<<结果是：0xffffff0f
	printf("b = 0x%x.\n", b);	// 结果是0xffffff00，说明~优先级高
	
	
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

















