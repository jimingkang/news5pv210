#include "shell.h"

// C��׼����Ҳ�и�memset�������������������õ����Լ�д�ģ�û�ñ�׼��
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

// ���û�������ַ�������str���տո�ָ��ɶ���ַ��������η���cmd��ά������
void cmdsplit(char cmd[][MAX_LEN_PART], const char *str)
{
	int m = 0, n = 0;	// m��ʾ��λ�����һά��n��ʾ�ڶ�ά
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
















