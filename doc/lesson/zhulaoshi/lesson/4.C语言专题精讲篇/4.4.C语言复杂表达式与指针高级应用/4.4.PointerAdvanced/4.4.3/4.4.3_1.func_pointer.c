#include <stdio.h>

int add(int a, int b);
int sub(int a, int b);
int multiply(int a, int b);
int divide(int a, int b);

// 定义了一个类型pFunc，这个函数指针类型指向一种特定参数列表和返回值的函数
typedef int (*pFunc)(int, int);


int main(void)
{
	pFunc p1 = NULL;
	char c = 0;
	int a = 0, b = 0, result = 0;
	
	printf("请输入要操作的2个整数:\n");
	scanf("%d %d", &a, &b);
	
	printf("请输入操作类型：+ | - | * | /\n");
	
	do 
	{
		scanf("%c", &c);
	}while (c == '\n');
	// 加一句调试
	//printf("a = %d, b = %d, c = %d.\n", a, b, c);
	
	switch (c)
	{
	case '+':
		p1 = add; break;
	case '-':
		p1 = sub; break;
	case '*':
		p1 = multiply; break;
	case '/':
		p1 = divide; break;
	default:
		p1 = NULL;	break;
	}
	
	
	result = p1(a, b);
	printf("%d %c %d = %d.\n", a, c, b, result);
	
	return 0;
}





int add(int a, int b)
{
	return a + b;
}

int sub(int a, int b)
{
	return a - b;
}

int multiply(int a, int b)
{
	return a * b;
}

int divide(int a, int b)
{
	return a / b;
}









