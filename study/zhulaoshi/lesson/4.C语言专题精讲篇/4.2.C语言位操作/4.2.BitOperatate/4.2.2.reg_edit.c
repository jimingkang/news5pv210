#include <stdio.h>


int main(void)
{
	/*
	// 把一个寄存器值的bit4～bit7取反，其他位不变
	unsigned int a = 0x123d0c37;
	unsigned int b = 0xf0;
	unsigned int c;
	
	c = a ^ b;
	printf("a & b = 0x%x.\n", c);	
	*/
	
	/*
	// 把一个寄存器值的bit4～bit7置1，其他位不变
	unsigned int a = 0x123d0cd7;
	unsigned int b = 0xf0;
	unsigned int c;
	
	c = a | b;
	printf("a & b = 0x%x.\n", c);	
	*/
	
	/*
	// 把一个寄存器值的bit13～21清0，其他位不变
	unsigned int a = 0x123d0c57;
	unsigned int b = 0xffc01fff;
	unsigned int c;
	
	c = a & b;
	printf("a & b = 0x%x.\n", c);		// 0xaaaa00aa
	*/
	/*
	unsigned int a = 0x12aaaaa7;
	unsigned int b = 0xFFFF00FF;
	unsigned int c;
	
	c = a & b;
	printf("a & b = 0x%x.\n", c);		// 0xaaaa00aa
	*/
}
