#include "shell.h"


static void shell_init(void)
{
	// shell init
	init_cmd_set();
	uart_init();
	puts("x210 simple shell:\n");		// shell logo
}

int main(void)
{
	char buf[MAX_LINE_LENGTH] = {0};		// �����ݴ��û����������
	
	shell_init();

	while(1)
	{
		// ��1���������ȡ
		puts("aston#");
		memset(buf, 0, sizeof(buf));		// bufŪ�ɾ��ô洢����û�����
		gets(buf);							// ��ȡ�û��������buf��
		//puts("��������ǣ�");
		//puts(buf);
		//puts("\n");
		// ��2�����������
		cmd_parser(buf);
		// ��3��������ִ��
		cmd_exec();
	}
	
	return 0;
}

