#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int a = 5;
int b;
int c = 0;
int array[1000];
	
char str[] = "linux";		// �ڶ��ַ����������ȫ�ֱ������������ݶ�


int main(void)
{
	char a[] = "linux";		// ��һ�ַ���������ɾֲ�����������ջ��
	
	char *p = (char *)malloc(10);	
	if (NULL == p)
	{
		printf("malloc error.\n");
		return -1;
	}
	memset(p, 0, 10);		// �����ַ����� ����malloc����Ķ��ڴ���
	strcpy(p, "linux");
	
	printf("%s\n", a);
	printf("%s\n", str);
	printf("%s\n", p);
	printf("%p\n", a);
	printf("%p\n", str);
	printf("%p\n", p);
	
	
/*	
	char *p = "linux";
	// const char *p = "linux";		// ����ȷ��д��Ӧ�������
	//*(p+0) = 'f';			// flinux
	
	printf("p = %s.\n", p);
	
	printf("p = %p.\n", p);
	printf("&a = %p.\n", &a);
*/

	
	return 0;
}
























