#include "stdio.h"
#include "main.h"

// ��������Ĵ����ĺ�
#define GPH0CON		0xE0200C00
#define GPH0DAT		0xE0200C04
#define GPH2CON		0xE0200C40
#define GPH2DAT		0xE0200C44

#define EXT_INT_0_CON	0xE0200E00
#define EXT_INT_2_CON	0xE0200E08
#define EXT_INT_0_PEND	0xE0200F40
#define EXT_INT_2_PEND	0xE0200F48
#define EXT_INT_0_MASK	0xE0200F00
#define EXT_INT_2_MASK	0xE0200F08


#define rGPH0CON	(*(volatile unsigned int *)GPH0CON)
#define rGPH0DAT	(*(volatile unsigned int *)GPH0DAT)
#define rGPH2CON	(*(volatile unsigned int *)GPH2CON)
#define rGPH2DAT	(*(volatile unsigned int *)GPH2DAT)

#define rEXT_INT_0_CON (*(volatile unsigned int *)EXT_INT_0_CON)
#define rEXT_INT_2_CON (*(volatile unsigned int *)EXT_INT_2_CON)
#define rEXT_INT_0_PEND (*(volatile unsigned int *)EXT_INT_0_PEND)
#define rEXT_INT_2_PEND (*(volatile unsigned int *)EXT_INT_2_PEND)
#define rEXT_INT_0_MASK (*(volatile unsigned int *)EXT_INT_0_MASK)
#define rEXT_INT_2_MASK (*(volatile unsigned int *)EXT_INT_2_MASK)

#define FALLING_EDGE_TRIGGE		2
#define RISING_EDGE_TRIGGE		3
#define BOTH_EDGE_TRIGGE		4

// ��ʼ������
void key_init(void)
{
	// ����GPHxCON�Ĵ���������Ϊ����ģʽ
	// GPH0CON��bit8��15ȫ������Ϊ0������
	rGPH0CON &= ~(0xFF<<8);
	// GPH2CON��bit0��15ȫ������Ϊ0������
	rGPH2CON &= ~(0xFFFF<<0);
}


void delay20ms(void)
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

//------------------------------------------------------------------

void key_init_int(void)
{
	rGPH0CON &= ~(0xFF<<8);
	rGPH0CON |= (0xFF<<8);
	rGPH2CON &= ~(0xFFFF<<0);
	rGPH2CON |= (0xFFFF<<0);
}

void eint_init(void)
{
	key_init_int();
	
	rEXT_INT_0_CON &= ~((7<<8) | (7<<12));
	rEXT_INT_0_CON |= ((FALLING_EDGE_TRIGGE<<8) | (FALLING_EDGE_TRIGGE<<12));
	
	rEXT_INT_2_CON &= ~(0xffff<<0);
	rEXT_INT_2_CON |= ((FALLING_EDGE_TRIGGE<<0)|(FALLING_EDGE_TRIGGE<<4)|(FALLING_EDGE_TRIGGE<<8)|(FALLING_EDGE_TRIGGE<<12));
	
	rEXT_INT_0_PEND &= ~(1<<2 | 1<<3);
	rEXT_INT_2_PEND &= ~(0x0f<<0);
	
	rEXT_INT_0_MASK &= ~(3<<2);
	rEXT_INT_2_MASK &= ~(0x0f<<0);
}

void key_isr_eint2(void)
{
	intc_clearvectaddr();
	rEXT_INT_0_PEND |= (1<<2);
	
	printf("eint2 key down.\n");
}

void key_isr_eint3(void)
{
	intc_clearvectaddr();
	rEXT_INT_0_PEND |= (1<<3);
	
	printf("eint3 key down.\n");
}

void key_isr_eint16171819(void)
{
	if (rEXT_INT_2_PEND & (1<<0))
	{
		printf("eint16 key down.\n");
	}
	
	if (rEXT_INT_2_PEND & (1<<1))
	{
		printf("eint17 key down.\n");
	}
	
	if (rEXT_INT_2_PEND & (1<<2))
	{
		printf("eint18 key down.\n");
	}
	
	if (rEXT_INT_2_PEND & (1<<3))
	{
		printf("eint19 key down.\n");
	}
	
	intc_clearvectaddr();
	rEXT_INT_2_PEND |= (0x0f<<0);
}



















