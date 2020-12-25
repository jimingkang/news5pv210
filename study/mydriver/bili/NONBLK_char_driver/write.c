#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
 
#include <string.h>
int main(){
int fd=0;
int ret;
char *s="world";
//fd=open("/dev/test/mydev",O_RDWR);
fd=open("/dev/mydev",O_RDWR);
if(fd<0){
        perror("open");
        exit(1);

}

ret=write(fd,s,strlen(s));

close(fd);
printf("app write %s\n",s);
return 0;
}
