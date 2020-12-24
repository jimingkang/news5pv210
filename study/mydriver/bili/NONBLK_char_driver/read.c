#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
 
#include <string.h>
int main(){
int fd=0;
int ret;
char buf[256];
char *s="hello";
fd=open("/dev/test/mydev",O_RDWR);
ret=read(fd,buf,sizeof(buf));
//write(fd,s,strlen(s));
close(fd);
printf("buf=%s\n",buf);
printf("ret=%d\n",ret);
return 0;
}
