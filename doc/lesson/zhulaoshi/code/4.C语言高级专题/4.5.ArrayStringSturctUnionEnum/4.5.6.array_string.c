#include <stdio.h>
#include <string.h>			// for strlen


int mystrlen(const char *p)
{
	int cnt = 0;
	while (*p++ != '\0')
	{
		cnt++;
		//p++;
	}
	return cnt;
}

char b[5];


int main(void)
{
	// ×Ö·û´®´æÔÚÕ»ÉÏ
	char a[7];	
	char *p = a;
	
	// ×Ö·û´®´æÔÚÊý¾Ý¶Î
	char *p = b;
	
	// ×Ö·û´®´æÔÚ¶Ñ¿Õ¼ä
	char *p = (char *)malloc(5);
	
/*	
	char *p = "linuxddd";
	printf("sizeof(p) = %d.\n", sizeof(p));		// 4
	printf("strlen(p) = %d.\n", strlen(p));		// 8
*/	
	
/*
	char a[7] = "windows";			// a[0] = 'w', a[1] = 'i', ````a[6] = 's'
	printf("sizeof(a) = %d.\n", sizeof(a));		// 7
	printf("strlen(a) = %d.\n", strlen(a));		// >=7
*/
	
/*
	char a[] = "windows";			// a[0] = 'w', a[1] = 'i', ````a[6] = 's', a[7] = '\0'
	printf("sizeof(a) = %d.\n", sizeof(a));		// 8
	printf("strlen(a) = %d.\n", strlen(a));		// 7
*/	
/*	
	char a[5] = "windows";
	printf("sizeof(a) = %d.\n", sizeof(a));		// 5
	printf("strlen(a) = %d.\n", strlen(a));		// 5
*/
	
/*
	char a[5] = "lin";
	printf("sizeof(a) = %d.\n", sizeof(a));		// 5
	printf("strlen(a) = %d.\n", strlen(a));		// 3
*/
/*
	char a[5] = {2, 3};
	printf("sizeof(a) = %d.\n", sizeof(a));		// 5
	printf("strlen(a) = %d.\n", strlen(a));		// 2
*/	
/*
	char a[5] = {0};
	printf("sizeof(a) = %d.\n", sizeof(a));		// 5
	printf("strlen(a) = %d.\n", strlen(a));		// 0
*/
/*	
	char a[5];
	printf("sizeof(a) = %d.\n", sizeof(a));		// 5
	printf("strlen(a) = %d.\n", strlen(a));		// 5
*/	
	
/*
	char *p = "linux";
	//int len = strlen(p);
	int len = mystrlen(p);
	printf("len = %d.\n", len);
*/	
	
	return 0;
}

















