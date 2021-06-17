#include "shell.h"
#include "stdio.h"
#include "int.h"

int g_isgo = 0;				// �������0���ܼ���ִ�У�����1����Լ���ִ����
int g_bootdelay = 3;		// �ȴ�ʱ��

static void hardware_init(void)
{
	led_init();				// led��ʼ��
	lcd_init();				// lcd��ʼ��
	timer2_pwm_init();		// ��������ʼ��
	adc_init();				// adc��ʼ��
}

// ȫ�ֱ�����ʼ��
void global_init(void)
{
	char val[20] = {0};
	if (env_get("bootdelay", val))
	{
		printf("env bootdelay not found.\n");
		return;
	}
	// �ַ���ת���֣��ַ�����ʽ��"5"ת�����ָ�ʽ��5��ֵ��g_bootdelay
	g_bootdelay = my_strtoul(val, (char *)0, 10);
}


static void shell_init(void)
{
	// shell init
	init_cmd_set();
	uart_init();
	hardware_init();
	wdt_timer_init();					// ���Ź�
	
	// ����������ʼ��
	env_init();
	global_init();

	puts("x210 simple shell:\n");		// shell logo
}

static void shell_loop(void)
{
	char buf[MAX_LINE_LENGTH] = {0};		// �����ݴ��û����������
	
	while(1)
	{
		// ��1���������ȡ
		puts("aston#");
		memset(buf, 0, sizeof(buf));		// bufŪ�ɾ��ô洢����û�����
		gets(buf);							// ��ȡ�û��������buf��
		//puts("��������ǣ�");
		//puts(buf);
		//puts("\n");
		// ��2�����������
		cmd_parser(buf);
		// ��3��������ִ��
		cmd_exec();
	}
}


int main(void)
{
	shell_init();
	
	// �Զ�����ִ��Ĭ������
	// ������ȴ��û������������û���͵���ʱ��������˾ͽ�������ʱ���Զ�ִ��ֱ��
	// ����shell��ѭ�������һֱû������ʱ�䵽��Ҳ����shell��ѭ��
	puts("aston#");
	printf("%d", g_bootdelay);
	//printf("g_isgo = %d\n", g_isgo);
	while ((!g_isgo) && (!is_key_press()));
	//while (!((g_isgo) || (is_key_press())));
	
	intc_disable(NUM_WDT);
	// Ҳ����������ͨ���ж�g_isgo��ֵ���ж��ǲ��ǵ���������ִ���Զ�����
	if (g_isgo)
	{
		lcd_test();
	}

	// ִ��shell��ѭ��
	shell_loop();
	
	
	return 0;
}

