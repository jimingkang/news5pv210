#include <stdio.h>




int main(void)
{
	int a[2][5] = {{1, 2, 3, 4, 5}, {6, 7, 8, 9, 10}};
	//int a[2][5] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	
	int *p = a;		// 类型不匹配
	
	printf("a[1][3] = %d.\n", a[1][3]);
	printf("a[1][3] = %d.\n", *(*(a+1)+3));
	
	
	return 0;
}
















