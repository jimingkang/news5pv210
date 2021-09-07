#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
int main(int argc, char **argv)
{
	int fd;
        unsigned char *val;
	val=(char *)malloc(12);
	fd = open("/dev/mpu6050", O_RDWR);
	if (fd < 0)
		printf("can't open /dev/6050\n");
	else
		printf("can open /dev/6050\n");
while(1)
{
	read(fd,val,12);	
 printf("app ax = %d \n", ((unsigned short )val[0]<<8) |val[1]);
                printf("app ay = %d \n", ((unsigned short)val[2]<<8) | val[3]);
                printf("app az = %d \n", ((unsigned short)val[4]<<8) | val[5]);
}	
	
	return 0;
}
