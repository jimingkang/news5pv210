#include <stdio.h>
#include <string.h>


#define MAX_LINE_LENGTH		256			// 命令行长度，命令不能超过这个长度


int main(void)
{
	char str[MAX_LINE_LENGTH];			// 用来存放用户输入的命令内容
	
	while (1)
	{
		// 打印命令行提示符，注意不能加换行
		printf("aston#");
		// 清除str数组以存放新的字符串
		memset(str, 0, sizeof(str));
		// shell第一步：获取用户输入的命令
		scanf("%s", str);
		// shell第二步：解析用户输入命令
		
		// shell第三步：处理用户输入命令
		printf("%s\n", str);
	}

	return 0;
}






















