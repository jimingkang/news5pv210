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
	// �ɹ���ȡ��stat�ṹ�壬���п��Եõ�����������Ϣ��
	printf("inode = %d.\n", buf.st_ino);
	printf("size = %d bytes.\n", buf.st_size);
	printf("st_blksize = %d.\n", buf.st_blksize);
	
	return 0;
}



















