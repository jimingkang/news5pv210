#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void func1(void)
{
	printf("func1\n");
}

void func2(void)
{
	printf("func2\n");
}


int main(void)
{
	printf("hello world.\n");
	
	// �����̱�������ֹʱ��ϵͳ���Զ���������ע���func1ִ��
	atexit(func2);
	atexit(func1);
	
	printf("my name is lilei hanmeimei\n");
	
	
	//return 0;
	//exit(0);
	_exit(0);
}







