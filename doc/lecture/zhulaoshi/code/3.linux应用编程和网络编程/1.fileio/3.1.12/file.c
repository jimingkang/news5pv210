#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


#define FILENAME	"1.txt"


int main(void)
{
	int fd1 = -1, fd2 = -1;
	
	fd1 = open(FILENAME, O_RDWR | O_CREAT | O_TRUNC, 0644);
	if (fd1 < 0)
	{
		perror("open");
		return -1;
	}
	printf("fd1 = %d.\n", fd1);
	
	//close(1);		// 1���Ǳ�׼���stdout
	
	
	// �����ļ�������
	//fd2 = dup(fd1);		// fd2һ������1����Ϊǰ��ոչر���1����仰�Ͱ�
	// 1.txt�ļ��ͱ�׼����Ͱ������ˣ������Ժ��������׼�������Ϣ��
	// ���Ե�1.txt�в鿴�ˡ�
	
	fd2 = dup2(fd1, 16);
	printf("fd2 = %d.\n", fd2);
//	printf("this is for test");

	while (1)
	{
		write(fd1, "aa", 2);
		sleep(1);
		write(fd2, "bb", 2);
	}


	
	close(fd1);
	return -1;
}



















