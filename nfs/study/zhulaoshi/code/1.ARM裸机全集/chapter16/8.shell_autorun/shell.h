#ifndef __SHELL_H__
#define __SHELL_H__




// �궨��
#define MAX_LINE_LENGTH		256			// �����г��ȣ�����ܳ����������

// �궨��һЩ��׼����ĵ�һ����
#define led					"led"
#define lcd					"lcd"
#define pwm					"buzzer"
#define adc					"adc"
#define CMD_NUM				4			// ��ǰϵͳ����������� 

// �궨��һ�����������Ϣ
#define MAX_CMD_PART		5			// һ������������������			
#define MAX_LEN_PART		20			// ����ķֲ�����󳤶�



// ȫ�ֱ�������
extern char g_cmdset[CMD_NUM][MAX_LINE_LENGTH];
extern int g_isgo;				
extern int g_bootdelay;		// 



// Ӳ����غ�������
void putchar(char c);
void puts(const char *p);
char *gets(char *p);
void uart_init(void);
int is_key_press(void);

// �������/ִ�����
void init_cmd_set(void);
void cmd_parser(char *str);
void cmd_exec(void);

// �ַ����������
void memset(char *p, int val, int length);
void strcpy(char *dst, const char *src);
int strcmp(const char *cs, const char *ct);
void cmdsplit(char cmd[][MAX_LEN_PART], const char *str);

// ��Ӳ��������������صĺ���
// ��һ�����led
void led_init();
void led_on(void);
void led_off(void);

// �ڶ������lcd
void lcd_init();
void lcd_test(void);


// ���������buzzer
void timer2_pwm_init(void);
void buzzer_on(void);
void buzzer_off(void);

// ���ĸ����adc
void adc_init();
void adc_collect1(void);

// wdt��ʱ
void wdt_timer_init(void);



#endif