#include "string.h"
#include "shell.h"


env_t envset[MAX_ENV_NUM];			// ϵͳ���֧��10����������


void env_init(void)
{
	memset((char *)envset, 0, sizeof(envset));
	// ��һ����������
	strcpy(envset[0].env_name, "bootdelay");
	strcpy(envset[0].env_val, "5");
	envset[0].is_used = 1;
	
	// �ڶ�����������
	strcpy(envset[1].env_name, "bootcmd");
	strcpy(envset[1].env_val, "ttttt");
	envset[1].is_used = 1;
	
	// ����Ļ�������
}

// �ɹ��ҵ�env�򷵻�0��ʧ���򷵻�1.
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
			// �ҵ��˻�������
			strcpy(val, envset[i].env_val);
			return 0;
		}
	}
	return 1;
}

void env_set(const char *pEnv, const char *val)
{
	int i;
	// ����Ŀǰ��û�������������������о�ֱ�Ӹ�ֵ
	for (i=0; i<sizeof(envset)/sizeof(envset[0]); i++)
	{
		if (!envset[i].is_used)
		{
			continue;
		}
		if (!strcmp(envset[i].env_name, pEnv))
		{
			// �ҵ��˻�������
			strcpy(envset[i].env_val, val);
			return;
		}
	}
	// û�ҵ�����������������½�����
	for (i=0; i<sizeof(envset)/sizeof(envset[0]); i++)
	{
		if (envset[i].is_used)
		{
			continue;
		}
		// �ҵ���һ����λ��Ȼ���ڴ˴����뼴��
		strcpy(envset[i].env_name, pEnv);
		strcpy(envset[i].env_val, val);
		envset[i].is_used = 1;
		return;
	}
	
	// �ұ��˻������������飬����û�п�λ��˵���Ѿ�������
	printf("env array is full.\n");
/*
	if (i >= sizeof(envset)/sizeof(envset[0])
	{
		// �ұ��˻������������飬����û�п�λ��˵���Ѿ�������
		printf("env array is full.\n");
	}
*/
}















