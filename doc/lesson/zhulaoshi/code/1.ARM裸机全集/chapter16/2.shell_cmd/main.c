#include <stdio.h>
#include <string.h>


#define MAX_LINE_LENGTH		256			// 命令行长度，命令不能超过这个长度



// 宏定义一些标准命令
#define led					"led"
#define lcd					"lcd"
#define pwm					"pwm"
#define CMD_NUM				3			// 当前系统定义的命令数 

char g_cmdset[CMD_NUM][MAX_LINE_LENGTH];


// 初始化命令列表
static void init_cmd_set(void)
{
	memset(g_cmdset, 0, sizeof(g_cmdset));		// 先全部清零
	strcpy(g_cmdset[0], led);
	strcpy(g_cmdset[1], lcd);
	strcpy(g_cmdset[2], pwm);
}




int main(void)
{
	int i = 0;
	char str[MAX_LINE_LENGTH];			// 用来存放用户输入的命令内容
	
	init_cmd_set();
	
	while (1)
	{
		// 打印命令行提示符，注意不能加换行
		printf("aston#");
		// 清除str数组以存放新的字符串
		memset(str, 0, sizeof(str));
		// shell第一步：获取用户输入的命令
		scanf("%s", str);
		// shell第二步：解析用户输入命令
		for (i=0; i<CMD_NUM; i++)
		{
			if (!strcmp(str, g_cmdset[i]))
			{
				// 相等，找到了这个命令，就去执行这个命令所对应的动作。
				printf("您输入的命令是：%s，是合法的\n", str);
				break;
			}	
		}
		if (i >= CMD_NUM)
		{
			// 找遍了命令集都没找到这个命令
			printf("%s不是一个内部合法命令，请重新输入\n", str);
		}
		
		// 第三步的处理已经移到前面分开处理了，所以这里不需要了
		// shell第三步：处理用户输入命令
	}

	return 0;
}





















