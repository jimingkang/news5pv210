#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
	int fd;
	char val;
	fd = open("/dev/mpu6050", O_RDWR);
	if (fd < 0)
		printf("can't open /dev/6050\n");
	else
		printf("can open /dev/6050\n");
	read(fd,&val,1);	
	return 0;
}
