#include <stdio.h>

//#define DEBUG
#undef DEBUG		// ע��һ���ꡣ���ǰ���ж����������ȡ�������

#ifdef DEBUG
#define debug(x)	printf(x)
#else
#define debug(x)	
#endif





int max(int a, int b);

#define MAX(a, b) (((a)>(b)) ? (a) : (b))

int max(int a, int b)
{
	if (a > b)
		return a;
	else
		return b;
}

int main(void)
{
	debug("this is a debug info. \n");
	
	
/*	
	float a, b, c;
	a = 1.3;
	b = 4.5;
	//c = max(&a, b);	
	c = MAX(&a, &b);
	printf("c = %f.\n", c);
*/
/*
	int a = 3, b = 5, c = 0;
	c = MAX(a, b);				// չ����c = (((a)>(b)) ? (a) : (b));
	c = max(a, b);				// �޷�չ����ֻ�ܵ���
	printf("c = %d.\n", c);
*/
	
	return 0;
}


















