#include <stdio.h>

#define M     	10  
#define N 		M

#define X(a, b)	((a)*(b))

#define MAX(a, b) (((a)>(b)) ? (a) : (b))


//#define SEC_PER_YEAR	(31506000)			// ���У����ǲ��Ƽ�
//#define SEC_PER_YEAR	(365*24*60*60)		// ����Ĭ��int���ͣ��������ʹ洢��Χ��
#define SEC_PER_YEAR	(365*24*60*60)UL	// ʵ�ʲ���gcc�б������
//#define SEC_PER_YEAR	(365*24*60*60UL)	// ʵ�ʲ�����ȷ



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







































