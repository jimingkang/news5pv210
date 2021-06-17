// �������������ִ����صĺ���
#include "shell.h"


char g_cmdset[CMD_NUM][MAX_LINE_LENGTH];		// �������������
char cmd[MAX_CMD_PART][MAX_LEN_PART];			// ��ǰ��������������
int cmd_index = -1;								// �洢�������������ǵڼ���������




/****************************����Ӳ�������������*************************/

// ����û�ҵ�������
void do_cmd_notfound(void)
{
	puts(cmd[0]);
	puts("����һ���ڲ��Ϸ��������������\n");
	puts("\n");
}

// led����Ĵ�����
void do_cmd_led(void)
{
	int flag = -1;
	//puts("led cmd");
	// ������led����Ĳ���ʵ��
	// Ŀǰ֧�ֵ�������led on | led off 
	// cmd[0]������led��cmd[1]������on|off
	if (!strcmp(cmd[1], "on"))
	{
		// led on
		led_on();
		flag = 1;
	}
	if (!strcmp(cmd[1], "off"))
	{
		// led off
		led_off();
		flag = 1;
	}
	// ..... �����Լ�����չ
	
	if (-1 == flag)
	{
		// ���һ����ûƥ�䣬���ӡʹ�÷���
		puts("command error, try: led on | led off");
		puts("\n");
	}
}

// �������������
void do_cmd_buzzer(void)
{
	int flag = -1;
	//puts("led cmd");
	// ������buzzer����Ĳ���ʵ��
	// Ŀǰ֧�ֵ�������buzzer on | buzzer off 
	// cmd[0]������buzzer��cmd[1]������on|off
	if (!strcmp(cmd[1], "on"))
	{
		// buzzer on
		buzzer_on();
		flag = 1;
	}
	if (!strcmp(cmd[1], "off"))
	{
		// buzzer off
		buzzer_off();
		flag = 1;
	}
	// ..... �����Լ�����չ
	
	if (-1 == flag)
	{
		// ���һ����ûƥ�䣬���ӡʹ�÷���
		puts("command error, try: buzzer on | buzzer off");
		puts("\n");
	}
}

// lcd�������

// ADC�������









/*********************************shell �������ִ�п��***********************/

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

// ִ������
void cmd_exec(void)
{
	switch (cmd_index)
	{
		case 0:		// led
			do_cmd_led();			break;
		case 1:		// lcd
		case 2:		// buzzer
			do_cmd_buzzer();		break;
		default:
			do_cmd_notfound();		break;
	}
}




















