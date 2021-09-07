#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>


#define SERADDR		"192.168.129.128"		// ���������Ÿ����ǵ�IP��ַ�Ͷ˿ں�
#define SERPORT		9003



int main(void)
{
	// ��1������socket���ļ�������
	int sockfd = -1, ret = -1;
	struct sockaddr_in seraddr = {0};
	struct sockaddr_in cliaddr = {0};
	
	// ��1����socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == sockfd)
	{
		perror("socket");
		return -1;
	}
	printf("socketfd = %d.\n", sockfd);
	
	// ��2����connect���ӷ�����
	seraddr.sin_family = AF_INET;		// ���õ�ַ��ΪIPv4
	seraddr.sin_port = htons(SERPORT);	// ���õ�ַ�Ķ˿ں���Ϣ
	seraddr.sin_addr.s_addr = inet_addr(SERADDR);	//������IP��ַ
	ret = connect(sockfd, (const struct sockaddr *)&seraddr, sizeof(seraddr));
	if (ret < 0)
	{
		perror("listen");
		return -1;
	}
	printf("connect result, ret = %d.\n", ret);
	
	
	return 0;
}