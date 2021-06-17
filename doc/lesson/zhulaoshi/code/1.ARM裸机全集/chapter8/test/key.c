#include "stdio.h"
#include "main.h"

// 定义操作寄存器的宏
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

// 初始化按键
void key_init(void)
{
	// 设置GPHxCON寄存器，设置为输入模式
	// GPH0CON的bit8～15全部设置为0，即可
	rGPH0CON &= ~(0xFF<<8);
	// GPH2CON的bit0～15全部设置为0，即可
	rGPH2CON &= ~(0xFFFF<<0);
}


void delay20ms(void)
{
	// 这个函数作用是延时20ms
	// 因为我们这里是裸机程序，且重点不是真的要消抖，而是教学
	// 所以我这里这个程序只是象征性的，并没有实体
	// 如果是研发，那就要花时间真的调试出延时20ms的程序
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
	// 依次，挨个去读出每个GPIO的值，判断其值为1还是0.如果为1则按键按下，为0则弹起
	
	// 轮询的意思就是反复循环判断有无按键，隔很短时间
	while (1)
	{
		// 对应开发板上标着LEFT的那个按键
		if (rGPH0DAT & (1<<2))
		{
			// 为1，说明没有按键
			led_off();
		}
		else
		{
			// 添加消抖
			// 第一步，延时
			delay20ms();
			// 第二步，再次检验按键状态
			if (!(rGPH0DAT & (1<<2)))
			{
				// 为0，说明有按键
				led1();
				printf("key left.\n");
			}
		}
		
		// 对应开发板上标着DOWN的那个按键
		if (rGPH0DAT & (1<<3))
		{
			// 为1，说明没有按键
			led_off();
		}
		else
		{
			// 为0，说明有按键
			led2();
			printf("key down.\n");
		}
		
		// 对应开发板上标着UP的那个按键
		if (rGPH2DAT & (1<<0))
		{
			// 为1，说明没有按键
			led_off();
		}
		else
		{
			// 为0，说明有按键
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



















