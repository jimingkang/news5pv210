#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


void create_daemon(void);


int main(void)
{
	create_daemon();
	
	
	while (1)
	{
		printf("I am running.\n");
		
		sleep(1);
	}
	
	return 0;
}


// �������þ��ǰѵ��øú����Ľ��̱��һ���ػ�����
void create_daemon(void)
{
	pid_t pid = 0;
	
	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		exit(-1);
	}
	if (pid > 0)
	{
		exit(0);		// ������ֱ���˳�
	}
	
	// ִ�е���������ӽ���
	
	// setsid����ǰ��������Ϊһ���µĻỰ��session��Ŀ�ľ����õ�ǰ����
	// �������̨��
	pid = setsid();
	if (pid < 0)
	{
		perror("setsid");
		exit(-1);
	}
	
	// ����ǰ���̹���Ŀ¼����Ϊ��Ŀ¼
	chdir("/");
	
	// umask����Ϊ0ȷ�����������������ļ�����Ȩ��
	umask(0);
	
	// �ر������ļ�������
	// ��Ҫ��ȡ��ǰϵͳ��������򿪵�����ļ���������Ŀ
	int cnt = sysconf(_SC_OPEN_MAX);
	int i = 0;
	for (i=0; i<cnt; i++)
	{
		close(i);
	}
	
	open("/dev/null", O_RDWR);
	open("/dev/null", O_RDWR);
	open("/dev/null", O_RDWR);

}




