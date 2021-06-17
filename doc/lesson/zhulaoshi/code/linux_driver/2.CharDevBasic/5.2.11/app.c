#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#define FILE	"/dev/test"			// 刚才mknod创建的设备文件名

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
	
	// 读写文件
	write(fd, "helloworld", 10);
	read(fd, buf, 100);
	
	
	// 关闭文件
	close(fd);
	
	return 0;
}