#include "test.h"
#include "stdio.h"

#define NUM 1


// 注释1
int main(void)
{
	int a = 0;
	#if (NUM == 0)		// 如果前面有定义NUM这个符号，成立
	a = 111;
	printf("#ifdef NUM.\n");
	#else			// 如果前面没有定义NUM这个符号，则执行下面的语句
	a = 222;
	printf("#elif.\n");
	#endif
	
	/*
	int a = 0;
	#ifdef NUM		// 如果前面有定义NUM这个符号，成立
	a = 111;
	printf("#ifdef NUM.\n");
	#else			// 如果前面没有定义NUM这个符号，则执行下面的语句
	a = 222;
	printf("#elif.\n");
	#endif
	
	printf("a = %d.\n", a);
*/	
	
	
	
	
	
	
	
	
	
	
	
	
	
	/*
		注释3
	
	*/
	//a = 3;
	printf("ddd");			// 注释2
	
	return 0;
}



