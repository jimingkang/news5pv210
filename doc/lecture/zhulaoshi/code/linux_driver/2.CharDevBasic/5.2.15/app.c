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
	write(fd, "helloworld2222", 14);
	read(fd, buf, 100);
	printf("读出来的内容是：%s.\n", buf);
	
	sleep(4);
	
	// 关闭文件
	close(fd);
	
	return 0;
}