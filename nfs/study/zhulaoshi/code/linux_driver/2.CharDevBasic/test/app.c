#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#define FILE	"/dev/test"			// �ղ�mknod�������豸�ļ���

char buf[100];

int main(void)
{
	int fd = -1;
	
	fd = open(FILE, O_RDWR);
	if (fd < 0)
	{
		printf("open %s error.\n", FILE);
		return -1;
	}
	printf("open %s success..\n", FILE);
	
	// ��д�ļ�
	write(fd, "sdfdddd", 7);
	read(fd, buf, 100);
	
	printf("���������ǣ�%s\n", buf);
	
	// �ر��ļ�
	close(fd);
	
	return 0;
}