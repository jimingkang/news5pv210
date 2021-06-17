#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

typedef void (*sighandler_t)(int);


void func(int sig)
{
	if (SIGINT != sig)
		return;
	
	printf("func for signal: %d.\n", sig);
}


int main(void)
{
	sighandler_t ret = (sighandler_t)-2;
	//signal(SIGINT, func);
	//signal(SIGINT, SIG_DFL);		// ָ���ź�SIGINTΪĬ�ϴ���
	ret = signal(SIGINT, SIG_IGN);		// ָ���ź�SIGINTΪ���Դ���
	if (SIG_ERR == ret)
	{
		perror("signal:");
		exit(-1);
	}
	
	printf("before while(1)\n");
	while(1);
	printf("after while(1)\n");
	
	return 0;
}