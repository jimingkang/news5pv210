#ifndef __SHELL_H__
#define __SHELL_H__




// �궨��
#define MAX_LINE_LENGTH		256			// �����г��ȣ�����ܳ����������

// �궨��һЩ��׼����ĵ�һ����
#define led					"led"
#define lcd					"lcd"
#define pwm					"pwm"
#define CMD_NUM				3			// ��ǰϵͳ����������� 

// �궨��һ�����������Ϣ
#define MAX_CMD_PART		5			// һ������������������			
#define MAX_LEN_PART		20			// ����ķֲ�����󳤶�



// ȫ�ֱ�������
extern char g_cmdset[CMD_NUM][MAX_LINE_LENGTH];



// Ӳ����غ�������
void puts(const char *p);
char *gets(char *p);
void uart_init(void);

// �������/ִ�����
void init_cmd_set(void);
void cmd_parser(char *str);
void cmd_exec(void);

// �ַ����������
void memset(char *p, int val, int length);
void strcpy(char *dst, const char *src);
int strcmp(const char *cs, const char *ct);
void cmdsplit(char cmd[][MAX_LEN_PART], const char *str);












#endif