#include "string.h"
#include "shell.h"


env_t envset[MAX_ENV_NUM];			// 系统最多支持10个环境变量


void env_init(void)
{
	memset((char *)envset, 0, sizeof(envset));
	// 第一个环境变量
	strcpy(envset[0].env_name, "bootdelay");
	strcpy(envset[0].env_val, "5");
	envset[0].is_used = 1;
	
	// 第二个环境变量
	strcpy(envset[1].env_name, "bootcmd");
	strcpy(envset[1].env_val, "ttttt");
	envset[1].is_used = 1;
	
	// 更多的环境变量
}

// 成功找到env则返回0，失败则返回1.
int env_get(const char *pEnv, char *val)
{
	int i;
	
	for (i=0; i<sizeof(envset)/sizeof(envset[0]); i++)
	{
		if (!envset[i].is_used)
		{
			continue;
		}
		if (!strcmp(envset[i].env_name, pEnv))
		{
			// 找到了环境变量
			strcpy(val, envset[i].env_val);
			return 0;
		}
	}
	return 1;
}

void env_set(const char *pEnv, const char *val)
{
	int i;
	// 先找目前有没有这个环境变量，如果有就直接改值
	for (i=0; i<sizeof(envset)/sizeof(envset[0]); i++)
	{
		if (!envset[i].is_used)
		{
			continue;
		}
		if (!strcmp(envset[i].env_name, pEnv))
		{
			// 找到了环境变量
			strcpy(envset[i].env_val, val);
			return;
		}
	}
	// 没找到这个环境变量，则新建即可
	for (i=0; i<sizeof(envset)/sizeof(envset[0]); i++)
	{
		if (envset[i].is_used)
		{
			continue;
		}
		// 找到了一个空位，然后在此处插入即可
		strcpy(envset[i].env_name, pEnv);
		strcpy(envset[i].env_val, val);
		envset[i].is_used = 1;
		return;
	}
	
	// 找遍了环境变量的数组，还是没有空位，说明已经存满了
	printf("env array is full.\n");
/*
	if (i >= sizeof(envset)/sizeof(envset[0])
	{
		// 找遍了环境变量的数组，还是没有空位，说明已经存满了
		printf("env array is full.\n");
	}
*/
}















