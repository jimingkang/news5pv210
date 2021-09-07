#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>


#define NAME "1.txt"

int main(void)
{
	int ret = -1;
	struct stat buf;
	
	memset(&buf, 0, sizeof(buf));		// memset��buf��ȫ��0
	ret = stat(NAME, &buf);				// stat��buf����������
	if (ret < 0)
	{
		perror("stat");
		exit(-1);
	}
	
#if 0	
	// �ж�����ļ�����
	//int result = S_ISREG(buf.st_mode);
	int result = S_ISDIR(buf.st_mode);
	printf("result = %d\n", result);
#endif

	// �ļ�Ȩ�޲���
	//unsigned int result = (buf.st_mode & S_IRWXU) >> 8;
	unsigned int result = ((buf.st_mode & S_IRUSR)? 1: 0);
	printf("file owner: %u.\n", result);
	
	
	
	
	return 0;
}



















