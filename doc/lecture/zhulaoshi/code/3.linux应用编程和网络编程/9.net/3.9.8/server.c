#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>



#define SERPORT		9003
#define SERADDR		"192.168.1.141"		// ifconfig看到的
#define BACKLOG		100


char recvbuf[100];


int main(void)
{
	// 第1步：先socket打开文件描述符
	int sockfd = -1, ret = -1, clifd = -1;
	socklen_t len = 0;
	struct sockaddr_in seraddr = {0};
	struct sockaddr_in cliaddr = {0};
	
	char ipbuf[30] = {0};
	
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == sockfd)
	{
		perror("socket");
		return -1;
	}
	printf("socketfd = %d.\n", sockfd);
	
	// 第2步：bind绑定sockefd和当前电脑的ip地址&端口号
	seraddr.sin_family = AF_INET;		// 设置地址族为IPv4
	seraddr.sin_port = htons(SERPORT);	// 设置地址的端口号信息
	seraddr.sin_addr.s_addr = inet_addr(SERADDR);	//　设置IP地址
	ret = bind(sockfd, (const struct sockaddr *)&seraddr, sizeof(seraddr));
	if (ret < 0)
	{
		perror("bind");
		return -1;
	}
	printf("bind success.\n");
	
	// 第三步：listen监听端口
	ret = listen(sockfd, BACKLOG);		// 阻塞等待客户端来连接服务器
	if (ret < 0)
	{
		perror("listen");
		return -1;
	}
	
	// 第四步：accept阻塞等待客户端接入
	clifd = accept(sockfd, (struct sockaddr *)&cliaddr, &len);
	printf("连接已经建立，client fd = %d.\n", clifd);
	
/*	
	// 建立连接之后就可以通信了
	// 客户端给服务器发
	ret = recv(clifd, recvbuf, sizeof(recvbuf), 0);
	printf("成功接收了%d个字节\n", ret);
	printf("client发送过来的内容是：%s\n", recvbuf);
*/

/*
	// 客户端反复给服务器发
	while (1)
	{
		ret = recv(clifd, recvbuf, sizeof(recvbuf), 0);
		//printf("成功接收了%d个字节\n", ret);
		printf("client发送过来的内容是：%s\n", recvbuf);	
		memset(recvbuf, 0, sizeof(recvbuf));
	}
*/
	// 服务器给客户端发
	strcpy(recvbuf, "hello world.");
	ret = send(clifd, recvbuf, strlen(recvbuf), 0);
	printf("发送了%d个字符\n", ret);

	
	return 0;
}


