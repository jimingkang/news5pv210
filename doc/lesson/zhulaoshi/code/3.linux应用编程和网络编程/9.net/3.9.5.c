#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define IPADDR	"192.168.1.102"

// 0x66		01	a8		c0
// 102		1	168		192
// �����ֽ�����ʵ���Ǵ��ģʽ


int main(void)
{
	struct in_addr addr = {0};
	char buf[50] = {0};
	
	addr.s_addr = 0x6703a8c0;
	
	inet_ntop(AF_INET, &addr, buf, sizeof(buf));

	printf("ip addr = %s.\n", buf);
	
	
/*	
	// ʹ��inet_pton��ת��
	int ret = 0;
	struct in_addr addr = {0};
	
	ret = inet_pton(AF_INET, IPADDR, &addr);
	if (ret != 1)
	{
		printf("inet_pton error\n");
		return -1;
	}
	
	printf("addr = 0x%x.\n", addr.s_addr);
*/	
	
	/*
	in_addr_t addr = 0;
	
	addr = inet_addr(IPADDR);
	
	printf("addr = 0x%x.\n", addr);		// 0x6601a8c0
*/	
	return 0;
}


