// 命令解析和命令执行相关的函数
#include "shell.h"


char g_cmdset[CMD_NUM][MAX_LINE_LENGTH];		// 命令集，存主命令
char cmd[MAX_CMD_PART][MAX_LEN_PART];			// 当前解析出来的命令
int cmd_index = -1;								// 存储解析到的命令是第几个主命令

// 初始化命令列表
void init_cmd_set(void)
{
	memset((char *)g_cmdset, 0, sizeof(g_cmdset));		// 先全部清零
	strcpy(g_cmdset[0], led);
	strcpy(g_cmdset[1], lcd);
	strcpy(g_cmdset[2], pwm);
	
	memset((char *)cmd, 0, sizeof(cmd));	
}

// 解析命令
void cmd_parser(char *str)
{
	int i;
	
	// 第一步，先将用户输入的次命令字符串分割放入cmd中
	cmdsplit(cmd, str);
	
	// 第二步，将cmd中的次命令第一个字符串和cmdset对比
	cmd_index = -1;
	for (i=0; i<CMD_NUM; i++)
	{
		// cmd[0]就是次命令中的第一个字符串，也就是主命令
		if (!strcmp(cmd[0], g_cmdset[i]))
		{
			// 相等，找到了这个命令，就去执行这个命令所对应的动作。
			//puts("您输入的命令是：");
			//puts(str);
			//puts("\n");
			cmd_index = i;
			
			break;
		}
	}			
/*		
		if (i >= CMD_NUM)
		{
			// 找遍了命令集都没找到这个命令
			cmd_index = -1;	
		}
*/	
}

// 命令没找到处理方法
void do_cmd_notfound(void)
{
	puts(cmd[0]);
	puts("不是一个内部合法命令，请重新输入\n");
	puts("\n");
}

// led命名的处理方法
void do_cmd_led(void)
{
	puts("led cmd");
}



// 执行命令
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




















