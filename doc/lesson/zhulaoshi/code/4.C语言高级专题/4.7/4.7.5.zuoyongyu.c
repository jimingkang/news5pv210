#include <stdio.h>

int a = 111;
extern int g_a;


void func(void)
{
	int a = 5;	// ������Χ��8-11��
	
	printf("in func, a = %d.\n", a);
}


int main(void)
{
	//printf("in main, g_a = %d.\n", g_a);
	
	// ��20-23�з�Χ�ڣ�44���a��111�Ǹ�a���ڱε���
	int a;			// a��������Χ����19�е�22��
	a = 44;
	printf("in main, a = %d.\n", a);
}

int g_a = 111;			// ��������4��֮��

























