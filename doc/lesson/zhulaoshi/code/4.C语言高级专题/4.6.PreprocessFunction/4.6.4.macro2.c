#include <stdio.h>

//#define DEBUG
#undef DEBUG		// 注销一个宏。如果前面有定义这个宏则取消这个宏

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
	c = MAX(a, b);				// 展开后：c = (((a)>(b)) ? (a) : (b));
	c = max(a, b);				// 无法展开，只能调用
	printf("c = %d.\n", c);
*/
	
	return 0;
}


















