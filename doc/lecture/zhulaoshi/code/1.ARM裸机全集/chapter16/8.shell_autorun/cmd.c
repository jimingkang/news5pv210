// 命令解析和命令执行相关的函数
#include "shell.h"


char g_cmdset[CMD_NUM][MAX_LINE_LENGTH];		// 命令集，存主命令
char cmd[MAX_CMD_PART][MAX_LEN_PART];			// 当前解析出来的命令
int cmd_index = -1;								// 存储解析到的命令是第几个主命令




/****************************具体硬件操作命令处理函数*************************/

// 命令没找到处理方法
void do_cmd_notfound(void)
{
	puts(cmd[0]);
	puts("不是一个内部合法命令，请重新输入\n");
	puts("\n");
}

// led命令的处理方法
void do_cmd_led(void)
{
	int flag = -1;
	//puts("led cmd");
	// 真正的led命令的操作实现
	// 目前支持的命令有led on | led off 
	// cmd[0]里面是led，cmd[1]里面是on|off
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
	// ..... 还可以继续扩展
	
	if (-1 == flag)
	{
		// 如果一个都没匹配，则打印使用方法
		puts("command error, try: led on | led off");
		puts("\n");
	}
}

// 蜂鸣器命令处理方法
void do_cmd_buzzer(void)
{
	int flag = -1;
	//puts("led cmd");
	// 真正的buzzer命令的操作实现
	// 目前支持的命令有buzzer on | buzzer off 
	// cmd[0]里面是buzzer，cmd[1]里面是on|off
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
	// ..... 还可以继续扩展
	
	if (-1 == flag)
	{
		// 如果一个都没匹配，则打印使用方法
		puts("command error, try: buzzer on | buzzer off");
		puts("\n");
	}
}

// lcd命令处理方法
void do_cmd_lcd(void)
{
	int flag = -1;

	// 真正的lcd命令的操作实现
	// 目前支持的命令有lcd test
	// cmd[0]里面是lcd，cmd[1]里面是test
	if (!strcmp(cmd[1], "test"))
	{
		// lcd test
		lcd_test();
		flag = 1;
	}
	// ..... 还可以继续扩展
	
	if (-1 == flag)
	{
		// 如果一个都没匹配，则打印使用方法
		puts("command error, try: lcd test");
		puts("\n");
	}
}

// ADC命令处理方法
void do_cmd_adc(void)
{
	int flag = -1;

	// 真正的adc命令的操作实现
	// 目前支持的命令有adc collect
	// cmd[0]里面是adc，cmd[1]里面是collect
	if (!strcmp(cmd[1], "collect"))
	{
		// led on
		adc_collect1();
		flag = 1;
	}
	// ..... 还可以继续扩展
	
	if (-1 == flag)
	{
		// 如果一个都没匹配，则打印使用方法
		puts("command error, try: adc collect");
		puts("\n");
	}
}



/*********************************shell 命令解析执行框架***********************/

// 初始化命令列表
void init_cmd_set(void)
{
	memset((char *)g_cmdset, 0, sizeof(g_cmdset));		// 先全部清零
	strcpy(g_cmdset[0], led);
	strcpy(g_cmdset[1], lcd);
	strcpy(g_cmdset[2], pwm);
	strcpy(g_cmdset[3], adc);
	
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

// 执行命令
void cmd_exec(void)
{
	switch (cmd_index)
	{
		case 0:		// led
			do_cmd_led();			break;
		case 1:		// lcd
			do_cmd_lcd();			break;
		case 2:		// buzzer	
			do_cmd_buzzer();		break;
		case 3:		// buzzer	
			do_cmd_adc();			break;
		default:
			do_cmd_notfound();		break;
	}
}




















