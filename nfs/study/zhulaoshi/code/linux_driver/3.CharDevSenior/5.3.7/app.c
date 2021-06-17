#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>


#define FILE	"/dev/test111"			// �ղ�mknod�������豸�ļ���

char buf[100];

int main(void)
{
	int fd = -1;
	int i = 0;
	
	fd = open(FILE, O_RDWR);
	if (fd < 0)
	{
		printf("open %s error.\n", FILE);
		return -1;
	}
	printf("open %s success..\n", FILE);

/*	
	// ��д�ļ�
	write(fd, "on", 2);
	sleep(2);
	write(fd, "off", 3);
	sleep(2);
	write(fd, "on", 2);
	sleep(2);
*/
/*
	write(fd, "1", 1);
	sleep(2);
	write(fd, "0", 1);
	sleep(2);
	write(fd, "1", 1);
	sleep(2);
*/
	while (1)
	{
		memset(buf, 0 , sizeof(buf));
		printf("������ on | off \n");
		scanf("%s", buf);
		if (!strcmp(buf, "on"))
		{
			write(fd, "1", 1);
		}
		else if (!strcmp(buf, "off"))
		{
			write(fd, "0", 1);
		}
		else if (!strcmp(buf, "flash"))
		{
			for (i=0; i<3; i++)
			{
				write(fd, "1", 1);
				sleep(1);
				write(fd, "0", 1);
				sleep(1);
			}
		}	
		else if (!strcmp(buf, "quit"))
		{
			break;
		}
	}

	
	// �ر��ļ�
	close(fd);
	
	return 0;
}