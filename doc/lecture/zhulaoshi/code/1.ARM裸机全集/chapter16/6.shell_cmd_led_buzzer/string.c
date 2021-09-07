#include "shell.h"

// C标准库中也有个memset函数，但是我们这里用的是自己写的，没用标准库
void memset(char *p, int val, int length)
{
	int i;
	
	for (i=0; i<length; i++)
	{
		p[i] = val;
	}
}

int strcmp(const char *cs, const char *ct)
{
	unsigned char c1, c2;

	while (1) {
		c1 = *cs++;
		c2 = *ct++;
		if (c1 != c2)
			return c1 < c2 ? -1 : 1;
		if (!c1)
			break;
	}
	return 0;
}

void strcpy(char *dst, const char *src)
{
	while (*src != '\0')
	{
		*dst++ = *src++;
	}
}

// 将用户输入的字符串命令str按照空格分隔成多个字符串，依次放入cmd二维数组中
void cmdsplit(char cmd[][MAX_LEN_PART], const char *str)
{
	int m = 0, n = 0;	// m表示二位数组第一维，n表示第二维
	while (*str != '\0')
	{
		if (*str != ' ')
		{
			cmd[m][n] = *str;
			n++;
		}
		else
		{
			cmd[m][n] = '\0';
			n = 0;
			m++;
		}
		str++;
	}
	cmd[m][n] = '\0';
}
















