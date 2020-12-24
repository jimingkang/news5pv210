#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
 
#include <string.h>
int main(){
int fd=0;
int ret;
char *s="hello";
fd=open("/dev/test/mydev",O_RDWR|O_NONBLOCK);
ret=write(fd,s,strlen(s));

close(fd);
printf("%s\n",s);
return 0;
}
