#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
 
#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <asm/types.h>
 
#include <linux/videodev2.h>
 
int device_open(char *dev_name)
{
    struct stat st;
    if (-1 == stat(dev_name, &st)) 
    {
        fprintf(stderr, "Cannot identify '%s': %d, %s\n", dev_name, errno, strerror(errno));
        // exit(EXIT_FAILURE);
    }
    if (!S_ISCHR(st.st_mode)) 
    {
        fprintf(stderr, "%s is no device\n", dev_name);
        // exit(EXIT_FAILURE);
    }
    int fd = open(dev_name, O_RDWR /* required */ | O_NONBLOCK, 0);
    if (-1 == fd) 
    {
        fprintf(stderr, "Cannot open '%s': %d, %s\n", dev_name, errno, strerror(errno));
        // exit(EXIT_FAILURE);
    }
    printf("%s %d : Open %s successfully. fd = %d\n", __func__, __LINE__, dev_name, fd);
	return fd;
}
 
int device_close(int fd)
{
    if (-1 == close(fd))
    {
    	printf("\tdevice close failed.\n");
    	// exit(EXIT_FAILURE);
    }
    else
    {
    	printf("%s %d : devices close successfully\n", __func__, __LINE__);
    }
	return 0;
}
 
int device_query(char *dev_name, int fd)
{
	struct v4l2_capability cap;
	memset(&cap, 0, sizeof(cap));
    if (-1 == ioctl(fd, VIDIOC_QUERYCAP, &cap)) // query v4l2-devices's capability
    {
        if (EINVAL == errno) 
        {
            fprintf(stderr, "%s is no V4L2 device\n", dev_name);
            // exit(EXIT_FAILURE);
        } 
        else 
        {
        	return 0;
        	printf("\tvideo ioctl_querycap failed.\n");
            // exit(EXIT_FAILURE);
        }
	}
	else//\tdevice name : %s\n    dev_name, 
	{
		printf("\n\tdriver name : %s\n\tcard name : %s\n\tbus info : %s\n\tdriver version : %u.%u.%u\n\n",
			    cap.driver, cap.card, cap.bus_info,(cap.version >> 16) & 0XFF, (cap.version >> 8) & 0XFF, cap.version & 0XFF);
	}
	return 0;
}
 
int dev_num = 27;
char dev_list[100][20] = {
	"/dev/video0",	
	"/dev/video3",	
};
 
int solve()
{
	int i = 0;
	int dev_start = 0;
	int dev_end = dev_num;
	for( i = dev_start; i < dev_end; ++i)
	{
		int fd = device_open(dev_list[i]);
		device_query(dev_list[i], fd);
		device_close(fd);
	}
	return 0;
}
 
 
int main()
{
	printf("\nstart game\n");
 
	solve();
 
	printf("\nquit game\n");
	return 0;
}
