#include <stdio.h>

#define M     	10  
#define N 		M

#define X(a, b)	((a)*(b))

#define MAX(a, b) (((a)>(b)) ? (a) : (b))


//#define SEC_PER_YEAR	(31506000)			// 可行，但是不推荐
//#define SEC_PER_YEAR	(365*24*60*60)		// 错误，默认int类型，超过类型存储范围了
#define SEC_PER_YEAR	(365*24*60*60)UL	// 实际测试gcc中编译错误
//#define SEC_PER_YEAR	(365*24*60*60UL)	// 实际测试正确



int main(void)
{
	unsigned int l = SEC_PER_YEAR;
	printf("l = %u.\n", l);
	//int x = 3, y = 4;
	//int max = MAX(2+x, y);
	//printf("max = %d.\n", max);
	
	//int a[N] = {1, 2, 3};
	//int b[10];
	//int x = 1, y = 2;
	
	//int c = 3 * X(x, 18+y);
	//printf("c = %d.\n", c);
	
	return 0;
}







































