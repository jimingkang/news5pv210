#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
 
#include <string.h>
int main(){
int fd=0;
char *s="hello";
fd=open("/dev/test/mydev",O_RDWR);
write(fd,s,strlen(s));

}
