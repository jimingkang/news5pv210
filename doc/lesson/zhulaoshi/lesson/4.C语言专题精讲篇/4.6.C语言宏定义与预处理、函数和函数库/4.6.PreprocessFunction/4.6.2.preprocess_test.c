#include "test.h"
#include "stdio.h"

#define NUM 1


// ע��1
int main(void)
{
	int a = 0;
	#if (NUM == 0)		// ���ǰ���ж���NUM������ţ�����
	a = 111;
	printf("#ifdef NUM.\n");
	#else			// ���ǰ��û�ж���NUM������ţ���ִ����������
	a = 222;
	printf("#elif.\n");
	#endif
	
	/*
	int a = 0;
	#ifdef NUM		// ���ǰ���ж���NUM������ţ�����
	a = 111;
	printf("#ifdef NUM.\n");
	#else			// ���ǰ��û�ж���NUM������ţ���ִ����������
	a = 222;
	printf("#elif.\n");
	#endif
	
	printf("a = %d.\n", a);
*/	
	
	
	
	
	
	
	
	
	
	
	
	
	
	/*
		ע��3
	
	*/
	//a = 3;
	printf("ddd");			// ע��2
	
	return 0;
}



