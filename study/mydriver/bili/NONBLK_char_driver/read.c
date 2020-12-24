#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
 
#include <string.h>
int main(){
int fd=0;
int ret;
char buf[256];
fd=open("/dev/mydev",O_RDWR);
if(fd<0){
	perror("open");
	exit(1);

}
ret=read(fd,buf,10);
if(ret<0){
	perror("read");
	exit(1);

}
//write(fd,s,strlen(s));
printf("fd=%d\n",fd);
close(fd);
printf("buf=%s\n",buf);
printf("ret=%d\n",ret);
return 0;
}
