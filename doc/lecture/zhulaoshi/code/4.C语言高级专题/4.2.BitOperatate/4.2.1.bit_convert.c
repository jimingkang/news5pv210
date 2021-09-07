#include <stdio.h>

int main(void)
{
	/*
	 *	45:101101
	 *	23:010111
	 *	58:111010		^操作
	 */
	
	// 位异或
	unsigned int a = 45, b = 23;
	unsigned int c;
	c = a ^ b;					// 58
	printf("c = %d.\n", c);
	
	/*
	// 按位和按逻辑取反再取反
	unsigned int a = 45;
	unsigned int b, c;
	b = ~~a;						// 按位取反，逐个位操作，1变0，0变1
	c = !!a;						// 按逻辑取反，真变假，假变真
	printf("b = %u.\n", b);
	printf("c = %d.\n", c);
	*/
	
	
	/*
	// 按位和按逻辑取反
	unsigned int a = 45;
	unsigned int b, c;
	b = ~a;						// 按位取反，逐个位操作，1变0，0变1
	c = !a;						// 按逻辑取反，真变假，假变真
	printf("b = %u.\n", b);
	printf("c = %d.\n", c);
	*/
	
	return 0;
}
