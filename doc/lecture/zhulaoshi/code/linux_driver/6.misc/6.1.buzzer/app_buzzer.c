#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>


#define DEVNAME		"/dev/buzzer"

#define PWM_IOCTL_SET_FREQ		1
#define PWM_IOCTL_STOP			0


int main(void)
{
	int fd = -1;
	
	fd = open(DEVNAME, O_RDWR);
	if (fd < 0)
	{
		perror("open");
		return -1;
	}
	
	ioctl(fd, PWM_IOCTL_SET_FREQ, 10000);
	sleep(3);
	ioctl(fd, PWM_IOCTL_STOP);
	sleep(3);
	ioctl(fd, PWM_IOCTL_SET_FREQ, 3000);
	sleep(3);
	ioctl(fd, PWM_IOCTL_STOP);
	sleep(3);
	
	
	close(fd);
	
	return 0;
}











