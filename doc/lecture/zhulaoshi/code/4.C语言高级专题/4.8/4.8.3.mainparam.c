#include <stdio.h>
#include <string.h>

// 通过给main传参，让程序运行的时候去选择执行哪个从而得到不同的执行结果
//int main(void)
int main(int argc, char *argv[])
{
	int i = 0;
	
	printf("main函数传参个数是：%d.\n", argc);
	for (i=0; i<argc; i++)
	{
		printf("第%d个参数是%s.\n", i, argv[i]);
	}
	
	if (argc != 2)
	{
		printf("we need 2 param.\n");
		return -1;
	}
	
	if (!strcmp(argv[1], "0"))
	{
		printf("boy\n");
	}
	else if (!strcmp(argv[1], "1"))
	{
		printf("girl\n");
	}
	else
	{
		printf("unknown\n");
	}
	
	
	
	return 0;
}












