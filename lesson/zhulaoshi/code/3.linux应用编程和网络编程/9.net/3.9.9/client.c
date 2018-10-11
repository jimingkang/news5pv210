#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>


#define SERADDR		"192.168.1.141"		// 服务器开放给我们的IP地址和端口号
#define SERPORT		9003


char sendbuf[100];
char recvbuf[100];


#define CMD_REGISTER	1001	// 注册学生信息
#define CMD_CHECK		1002	// 检验学生信息
#define CMD_GETINFO		1003	// 获取学生信息

#define STAT_OK			30		// 回复ok
#define STAT_ERR		31		// 回复出错了


typedef struct commu
{
	char name[20];		// 学生姓名
	int age;			// 学生年龄
	int cmd;			// 命令码
	int stat;			// 状态信息，用来回复
}info;



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
	while (1)
	{
		// 回合中第1步：客户端给服务器发送信息
		printf("请输入要发送的内容\n");
		scanf("%s", sendbuf);
		//printf("刚才输入的是：%s\n", sendbuf);
		ret = send(sockfd, sendbuf, strlen(sendbuf), 0);
		printf("发送了%d个字符\n", ret);
		
		// 回合中第2步：客户端接收服务器的回复
		memset(recvbuf, 0, sizeof(recvbuf));
		ret = recv(sockfd, recvbuf, sizeof(recvbuf), 0);
		//printf("成功接收了%d个字节\n", ret);
		printf("client发送过来的内容是：%s\n", recvbuf);

		// 回合中第3步：客户端解析服务器的回复，再做下一步定夺
		
	}
*/

	while (1)
	{
		// 回合中第1步：客户端给服务器发送信息
		info st1;
		printf("请输入学生姓名\n");
		scanf("%s", st1.name);
		printf("请输入学生年龄");
		scanf("%d", &st1.age);
		st1.cmd = CMD_REGISTER;
		//printf("刚才输入的是：%s\n", sendbuf);
		ret = send(sockfd, &st1, sizeof(info), 0);
		printf("发送了1个学生信息\n");
		
		// 回合中第2步：客户端接收服务器的回复
		memset(&st1, 0, sizeof(st1));
		ret = recv(sockfd, &st1, sizeof(st1), 0);
		
		// 回合中第3步：客户端解析服务器的回复，再做下一步定夺
		if (st1.stat == STAT_OK)
		{
			printf("注册学生信息成功\n");
		}
		else
		{
			printf("注册学生信息失败\n");
		}

	}



	return 0;
}





