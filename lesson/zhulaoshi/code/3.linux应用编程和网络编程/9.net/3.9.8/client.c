#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>


#define SERADDR		"192.168.1.141"		// 服务器开放给我们的IP地址和端口号
#define SERPORT		9003


char sendbuf[100];



int main(void)
{
	// 第1步：先socket打开文件描述符
	int sockfd = -1, ret = -1;
	struct sockaddr_in seraddr = {0};
	struct sockaddr_in cliaddr = {0};
	
	// 第1步：socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == sockfd)
	{
		perror("socket");
		return -1;
	}
	printf("socketfd = %d.\n", sockfd);
	
	// 第2步：connect链接服务器
	seraddr.sin_family = AF_INET;		// 设置地址族为IPv4
	seraddr.sin_port = htons(SERPORT);	// 设置地址的端口号信息
	seraddr.sin_addr.s_addr = inet_addr(SERADDR);	//　设置IP地址
	ret = connect(sockfd, (const struct sockaddr *)&seraddr, sizeof(seraddr));
	if (ret < 0)
	{
		perror("listen");
		return -1;
	}
	printf("成功建立连接\n");

/*	
	// 建立连接之后就可以开始通信了
	strcpy(sendbuf, "hello world.");
	ret = send(sockfd, sendbuf, strlen(sendbuf), 0);
	printf("发送了%d个字符\n", ret);
*/
/*
	while (1)
	{
		printf("请输入要发送的内容\n");
		scanf("%s", sendbuf);
		//printf("刚才输入的是：%s\n", sendbuf);
		ret = send(sockfd, sendbuf, strlen(sendbuf), 0);
		printf("发送了%d个字符\n", ret);
	}
*/
	ret = recv(sockfd, sendbuf, sizeof(sendbuf), 0);
	printf("成功接收了%d个字节\n", ret);
	printf("client发送过来的内容是：%s\n", sendbuf);
	
	
	return 0;
}





