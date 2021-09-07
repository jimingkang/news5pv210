#include <stdio.h>

int main(void)
{
	
	// 聪明的方法
	unsigned int a;
	a = ~(0x7f<<4);						// 0xfffff80f
	printf("a = 0x%x.\n", a);
	
	/*
	// 笨方法
	unsigned int a;
	a = (0xf<<0)|(0x1fffff<<11);		// 0xfffff80f
	printf("a = 0x%x.\n", a);
*/

	
	/*
	unsigned int a;
	
	// 下面表达式含义：位或说明这个数字由2部分组成，第一部分中左移3位说明第一部分从bit3开始，
	// 第一部分数字为0x1f说明这部分有5位，所以第一部分其实就是bit3到bit7；
	// 第二部分的解读方法同样的，可知第二部分其实就是bit23到bit25；
	// 所以两部分结合起来，这个数的特点就是：bit3～bit7和bit23～bit25为1，其余位全部为0.
	a = ((0x1f<<3) | (0x7<<23));
	printf("a = 0x%x.\n", a);			// 工具算出来：0x0380_00f8		程序运行：0x38000f8
	*/
	
	return 0;
}