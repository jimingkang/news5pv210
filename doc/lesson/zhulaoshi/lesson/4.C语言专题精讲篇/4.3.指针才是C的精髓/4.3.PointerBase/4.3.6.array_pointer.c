#include <stdio.h>


int main(void)
{
	int a[5] = {1, 2, 3, 4, 5};
	int *p;
	p = a;
	
	printf("*(p+1) = %d.\n", *(p+1));
	printf("*(p+1) = %d.\n", *((char *)p+1));
	printf("*(p+1) = %d.\n", *(int *)((unsigned int)p+1));
	
	char *p2;
	p2 = (char *)p;
	printf("*(p+1) = %d.\n", *(p2+1));
	
	
/*
	int a[5] = {1, 2, 3, 4, 5};
	int *p;
	p = &a;
	
	printf("a = %x.\n", a);
	printf("&a = %x.\n", &a);
	printf("&a[0] = %x.\n", &a[0]);
	printf("a[0] = %x.\n", a[0]);
*/
	
/*	
	int a[5] = {1, 2, 3, 4, 5};

	printf("a[3] = %d.\n", a[3]);
	printf("*(a+3) = %d.\n", *(a+3));
	//	��Ч�ڣ�int b = *(a+3); printf("*(a+3) = %d.\n", b);
	
	int *p;
	p = a;		// a����ֵ��ʾ������Ԫ���׵�ַ����ͬ��&a[0]
	printf("*(p+3) = %d.\n", *(p+3));		// ��ͬ��a[3]
	printf("*(p-1) = %d.\n", *(p-1));		// ��ͬ��a[-1]
	
	p = &a[2];
	printf("*(p+1) = %d.\n", *(p+1));		// ��ͬ��a[3]
	printf("*(p-1) = %d.\n", *(p-1));		// ��ͬ��a[1]
	printf("*(p+3) = %d.\n", *(p+3));		// ��ͬ��a[5]
*/	
	return 0;
}
















