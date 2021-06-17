#include "stdio.h"
#include "main.h"

// ��������Ĵ����ĺ�
#define GPH0CON		0xE0200C00
#define GPH0DAT		0xE0200C04
#define GPH2CON		0xE0200C40
#define GPH2DAT		0xE0200C44

#define rGPH0CON	(*(volatile unsigned int *)GPH0CON)
#define rGPH0DAT	(*(volatile unsigned int *)GPH0DAT)
#define rGPH2CON	(*(volatile unsigned int *)GPH2CON)
#define rGPH2DAT	(*(volatile unsigned int *)GPH2DAT)

#define EXT_INT_0_CON	0xE0200E00
#define EXT_INT_2_CON	0xE0200E08
#define EXT_INT_0_PEND	0xE0200F40
#define EXT_INT_2_PEND	0xE0200F48
#define EXT_INT_0_MASK	0xE0200F00
#define EXT_INT_2_MASK	0xE0200F08

#define rEXT_INT_0_CON	(*(volatile unsigned int *)EXT_INT_0_CON)
#define rEXT_INT_2_CON	(*(volatile unsigned int *)EXT_INT_2_CON)
#define rEXT_INT_0_PEND	(*(volatile unsigned int *)EXT_INT_0_PEND)
#define rEXT_INT_2_PEND	(*(volatile unsigned int *)EXT_INT_2_PEND)
#define rEXT_INT_0_MASK	(*(volatile unsigned int *)EXT_INT_0_MASK)
#define rEXT_INT_2_MASK	(*(volatile unsigned int *)EXT_INT_2_MASK)



//------------------------��ѯ��ʽ������---------------------------
// ��ʼ������
void key_init(void)
{
	// ����GPHxCON�Ĵ���������Ϊ����ģʽ
	// GPH0CON��bit8��15ȫ������Ϊ0������
	rGPH0CON &= ~(0xFF<<8);
	// GPH2CON��bit0��15ȫ������Ϊ0������
	rGPH2CON &= ~(0xFFFF<<0);
}

static void delay20ms(void)
{
	// ���������������ʱ20ms
	// ��Ϊ��������������������ص㲻�����Ҫ���������ǽ�ѧ
	// �����������������ֻ�������Եģ���û��ʵ��
	// ������з����Ǿ�Ҫ��ʱ����ĵ��Գ���ʱ20ms�ĳ���
	int i, j;
	
	for (i=0; i<100; i++)
	{
		for (j=0; j<1000; j++)
		{
			i * j;
		}
	}
}

void key_polling(void)
{
	// ���Σ�����ȥ����ÿ��GPIO��ֵ���ж���ֵΪ1����0.���Ϊ1�򰴼����£�Ϊ0����
	
	// ��ѯ����˼���Ƿ���ѭ���ж����ް��������ܶ�ʱ��
	while (1)
	{
		// ��Ӧ�������ϱ���LEFT���Ǹ�����
		if (rGPH0DAT & (1<<2))
		{
			// Ϊ1��˵��û�а���
			led_off();
		}
		else
		{
			// �������
			// ��һ������ʱ
			delay20ms();
			// �ڶ������ٴμ��鰴��״̬
			if (!(rGPH0DAT & (1<<2)))
			{
				// Ϊ0��˵���а���
				led1();
				printf("key left.\n");
			}
		}
		
		// ��Ӧ�������ϱ���DOWN���Ǹ�����
		if (rGPH0DAT & (1<<3))
		{
			// Ϊ1��˵��û�а���
			led_off();
		}
		else
		{
			// Ϊ0��˵���а���
			led2();
			printf("key down.\n");
		}
		
		// ��Ӧ�������ϱ���UP���Ǹ�����
		if (rGPH2DAT & (1<<0))
		{
			// Ϊ1��˵��û�а���
			led_off();
		}
		else
		{
			// Ϊ0��˵���а���
			led3();
		}
	}
}


//-----------------------�жϷ�ʽ������-----------------------------------
// ���жϷ�ʽ���������ĳ�ʼ��
void key_init_interrupt(void)
{
	// 1. �ⲿ�ж϶�Ӧ��GPIOģʽ����
	rGPH0CON |= 0xFF<<8;		// GPH0_2 GPH0_3����Ϊ�ⲿ�ж�ģʽ
	rGPH2CON |= 0xFFFF<<0;		// GPH2_0123��4����������Ϊ�ⲿ�ж�ģʽ
	
	// 2. �жϴ���ģʽ����
	rEXT_INT_0_CON &= ~(0xFF<<8);	// bit8~bit15ȫ������
	rEXT_INT_0_CON |= ((2<<8)|(2<<12));		// EXT_INT2��EXT_INT3����Ϊ�½��ش���
	rEXT_INT_2_CON &= ~(0xFFFF<<0);
	rEXT_INT_2_CON |= ((2<<0)|(2<<4)|(2<<8)|(2<<12));	
	
	// 3. �ж�����
	rEXT_INT_0_MASK &= ~(3<<2);			// �ⲿ�ж�����
	rEXT_INT_2_MASK &= ~(0x0f<<0);
	
	// 4. ����������д1������д0
	rEXT_INT_0_PEND |= (3<<2);
	rEXT_INT_2_PEND |= (0x0F<<0);
}

// EINT2ͨ����Ӧ�İ���������GPH0_2���Ŷ�Ӧ�İ��������ǿ������ϱ���LEFT���Ǹ�����
void isr_eint2(void)
{
	// ������isrӦ����2�����顣
	// ��һ���жϴ�����룬���������ɻ�Ĵ���
	printf("isr_eint2_LEFT.\n");
	// �ڶ�������жϹ���
	rEXT_INT_0_PEND |= (1<<2);
	intc_clearvectaddr();
}

void isr_eint3(void)
{
	// ������isrӦ����2�����顣
	// ��һ���жϴ�����룬���������ɻ�Ĵ���
	printf("isr_eint3_DOWN.\n");
	// �ڶ�������жϹ���
	rEXT_INT_0_PEND |= (1<<3);
	intc_clearvectaddr();
}

void isr_eint16171819(void)
{
	// ������isrӦ����2�����顣
	// ��һ���жϴ�����룬���������ɻ�Ĵ���
	// ��ΪEINT16��31�ǹ����жϣ�����Ҫ�������ٴ�ȥ���־������ĸ����ж�
	if (rEXT_INT_2_PEND & (1<<0))
	{
		printf("eint16\n");
	}
	if (rEXT_INT_2_PEND & (1<<1))
	{
		printf("eint17\n");
	}
	if (rEXT_INT_2_PEND & (1<<2))
	{
		printf("eint18\n");
	}
	if (rEXT_INT_2_PEND & (1<<3))
	{
		printf("eint19\n");
	}

	// �ڶ�������жϹ���
	rEXT_INT_2_PEND |= (0x0f<<0);
	intc_clearvectaddr();
}















