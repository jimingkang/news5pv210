#include "shell.h"
#include "stdio.h"
#include "int.h"

int g_isgo = 0;				// 如果等于0则不能继续执行，等于1则可以继续执行了
int g_bootdelay = 3;		// 等待时间

static void hardware_init(void)
{
	led_init();				// led初始化
	lcd_init();				// lcd初始化
	timer2_pwm_init();		// 蜂鸣器初始化
	adc_init();				// adc初始化
}

// 全局变量初始化
void global_init(void)
{
	char val[20] = {0};
	if (env_get("bootdelay", val))
	{
		printf("env bootdelay not found.\n");
		return;
	}
	// 字符串转数字，字符串格式的"5"转成数字格式的5赋值给g_bootdelay
	g_bootdelay = my_strtoul(val, (char *)0, 10);
}


static void shell_init(void)
{
	// shell init
	init_cmd_set();
	uart_init();
	hardware_init();
	wdt_timer_init();					// 看门狗
	
	// 环境变量初始化
	env_init();
	global_init();

	puts("x210 simple shell:\n");		// shell logo
}

static void shell_loop(void)
{
	char buf[MAX_LINE_LENGTH] = {0};		// 用来暂存用户输入的命令
	
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
}


int main(void)
{
	shell_init();
	
	// 自动倒数执行默认命令
	// 在这里等待用户按按键，如果没按就倒计时，如果按了就结束倒计时和自动执行直接
	// 进入shell死循环。如果一直没按按键时间到了也进入shell死循环
	puts("aston#");
	printf("%d", g_bootdelay);
	//printf("g_isgo = %d\n", g_isgo);
	while ((!g_isgo) && (!is_key_press()));
	//while (!((g_isgo) || (is_key_press())));
	
	intc_disable(NUM_WDT);
	// 也可以在这里通过判断g_isgo的值来判断是不是倒数结束，执行自动命令
	if (g_isgo)
	{
		lcd_test();
	}

	// 执行shell死循环
	shell_loop();
	
	
	return 0;
}

