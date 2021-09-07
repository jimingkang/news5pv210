#include "shell.h"


static void hardware_init(void)
{
	led_init();				// led初始化
	lcd_init();				// lcd初始化
	timer2_pwm_init();		// 蜂鸣器初始化
	adc_init();				// adc初始化
}


static void shell_init(void)
{
	// shell init
	init_cmd_set();
	uart_init();
	hardware_init();
	puts("x210 simple shell:\n");		// shell logo
}

int main(void)
{
	char buf[MAX_LINE_LENGTH] = {0};		// 用来暂存用户输入的命令
	
	shell_init();

	while(1)
	{
		// 第1步：命令获取
		puts("aston#");
		memset(buf, 0, sizeof(buf));		// buf弄干净好存储这次用户输入
		gets(buf);							// 读取用户输入放入buf中
		//puts("您输入的是：");
		//puts(buf);
		//puts("\n");
		// 第2步：命令解析
		cmd_parser(buf);
		// 第3步：命令执行
		cmd_exec();
	}
	
	return 0;
}

