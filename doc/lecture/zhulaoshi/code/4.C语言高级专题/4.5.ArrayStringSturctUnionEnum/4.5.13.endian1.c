#include <stdio.h>

// 共用体中很重要的一点：a和b都是从u1的低地址开始的。
// 假设u1所在的4字节地址分别是：0、1、2、3的话，那么a自然就是0、1、2、3；
// b所在的地址是0而不是3.
union myunion
{
	int a;
	char b;
};

// 如果是小端模式则返回1，小端模式则返回0
int is_little_endian(void)
{
	union myunion u1;
	u1.a = 1;				// 地址0的那个字节内是1（小端）或者0（大端）
	return u1.b;
}

int is_little_endian2(void)
{
	int a = 1;
	char b = *((char *)(&a));		// 指针方式其实就是共用体的本质
	
	return b;
}


int main(void)
{
	int i = is_little_endian2();
	if (i == 1)
	{
		printf("小端模式\n");
	}
	else
	{
		printf("大端模式\n");
	}
	
	return 0;
}



















