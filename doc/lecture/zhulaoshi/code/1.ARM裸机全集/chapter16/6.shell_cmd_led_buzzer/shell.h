#ifndef __SHELL_H__
#define __SHELL_H__




// 宏定义
#define MAX_LINE_LENGTH		256			// 命令行长度，命令不能超过这个长度

// 宏定义一些标准命令的第一部分
#define led					"led"
#define lcd					"lcd"
#define pwm					"buzzer"
#define CMD_NUM				3			// 当前系统定义的命令数 

// 宏定义一个命令相关信息
#define MAX_CMD_PART		5			// 一个命令最多包含几部分			
#define MAX_LEN_PART		20			// 命令的分部分最大长度



// 全局变量声明
extern char g_cmdset[CMD_NUM][MAX_LINE_LENGTH];



// 硬件相关函数声明
void puts(const char *p);
char *gets(char *p);
void uart_init(void);

// 命令解析/执行相关
void init_cmd_set(void);
void cmd_parser(char *str);
void cmd_exec(void);

// 字符串函数相关
void memset(char *p, int val, int length);
void strcpy(char *dst, const char *src);
int strcmp(const char *cs, const char *ct);
void cmdsplit(char cmd[][MAX_LEN_PART], const char *str);

// 各硬件操作的命令相关的函数
// 第一个命令：led
void led_init();
void led_on(void);
void led_off(void);

// 第三个命令：buzzer
void timer2_pwm_init(void);
void buzzer_on(void);
void buzzer_off(void);





#endif