// �������������ִ����صĺ���
#include "shell.h"


char g_cmdset[CMD_NUM][MAX_LINE_LENGTH];


// ��ʼ�������б�
void init_cmd_set(void)
{
	memset((char *)g_cmdset, 0, sizeof(g_cmdset));		// ��ȫ������
	strcpy(g_cmdset[0], led);
	strcpy(g_cmdset[1], lcd);
	strcpy(g_cmdset[2], pwm);
}

// ��������
void cmd_parser(char *str)
{
	int i;
	
	for (i=0; i<CMD_NUM; i++)
		{
			if (!strcmp(str, g_cmdset[i]))
			{
				// ��ȣ��ҵ�����������ȥִ�������������Ӧ�Ķ�����
				puts("������������ǣ�");
				puts(str);
				puts("\n");
				break;
			}	
		}
		if (i >= CMD_NUM)
		{
			// �ұ��������û�ҵ��������
			puts(str);
			puts("����һ���ڲ��Ϸ��������������\n");
			puts("\n");
		}
}

// ִ������
void cmd_exec(void)
{
	
}




















