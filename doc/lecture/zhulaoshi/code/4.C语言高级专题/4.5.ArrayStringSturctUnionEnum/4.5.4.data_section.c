#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int a = 5;
int b;
int c = 0;
int array[1000];
	
char str[] = "linux";		// 第二种方法：定义成全局变量，放在数据段


int main(void)
{
	char a[] = "linux";		// 第一种方法：定义成局部变量，放在栈上
	
	char *p = (char *)malloc(10);	
	if (NULL == p)
	{
		printf("malloc error.\n");
		return -1;
	}
	memset(p, 0, 10);		// 第三种方法： 放在malloc申请的堆内存中
	strcpy(p, "linux");
	
	printf("%s\n", a);
	printf("%s\n", str);
	printf("%s\n", p);
	printf("%p\n", a);
	printf("%p\n", str);
	printf("%p\n", p);
	
	
/*	
	char *p = "linux";
	// const char *p = "linux";		// 最正确的写法应该是这个
	//*(p+0) = 'f';			// flinux
	
	printf("p = %s.\n", p);
	
	printf("p = %p.\n", p);
	printf("&a = %p.\n", &a);
*/

	
	return 0;
}
























