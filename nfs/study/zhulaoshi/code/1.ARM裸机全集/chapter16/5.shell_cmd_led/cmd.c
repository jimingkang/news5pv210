// �������������ִ����صĺ���
#include "shell.h"


char g_cmdset[CMD_NUM][MAX_LINE_LENGTH];		// �������������
char cmd[MAX_CMD_PART][MAX_LEN_PART];			// ��ǰ��������������
int cmd_index = -1;								// �洢�������������ǵڼ���������

// ��ʼ�������б�
void init_cmd_set(void)
{
	memset((char *)g_cmdset, 0, sizeof(g_cmdset));		// ��ȫ������
	strcpy(g_cmdset[0], led);
	strcpy(g_cmdset[1], lcd);
	strcpy(g_cmdset[2], pwm);
	
	memset((char *)cmd, 0, sizeof(cmd));	
}

// ��������
void cmd_parser(char *str)
{
	int i;
	
	// ��һ�����Ƚ��û�����Ĵ������ַ����ָ����cmd��
	cmdsplit(cmd, str);
	
	// �ڶ�������cmd�еĴ������һ���ַ�����cmdset�Ա�
	cmd_index = -1;
	for (i=0; i<CMD_NUM; i++)
	{
		// cmd[0]���Ǵ������еĵ�һ���ַ�����Ҳ����������
		if (!strcmp(cmd[0], g_cmdset[i]))
		{
			// ��ȣ��ҵ�����������ȥִ�������������Ӧ�Ķ�����
			//puts("������������ǣ�");
			//puts(str);
			//puts("\n");
			cmd_index = i;
			
			break;
		}
	}			
/*		
		if (i >= CMD_NUM)
		{
			// �ұ��������û�ҵ��������
			cmd_index = -1;	
		}
*/	
}

// ����û�ҵ�������
void do_cmd_notfound(void)
{
	puts(cmd[0]);
	puts("����һ���ڲ��Ϸ��������������\n");
	puts("\n");
}

// led�����Ĵ�����
void do_cmd_led(void)
{
	puts("led cmd");
}



// ִ������
void cmd_exec(void)
{
	switch (cmd_index)
	{
		case 0:
			do_cmd_led();			break;
		case 1:
		case 2:
		default:
			do_cmd_notfound();		break;
	}
}




















