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
fd=open("/dev/mydev",O_RDWR|O_NONBLOCK);
ret=read(fd,buf,sizeof(buf));
//write(fd,s,strlen(s));
close(fd);
printf("app read noblk %s\n",buf);
return 0;
}
