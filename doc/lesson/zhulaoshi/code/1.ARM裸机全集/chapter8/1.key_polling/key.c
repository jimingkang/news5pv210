// 定义操作寄存器的宏
#define GPH0CON		0xE0200C00
#define GPH0DAT		0xE0200C04

#define GPH2CON		0xE0200C40
#define GPH2DAT		0xE0200C44

#define rGPH0CON	(*(volatile unsigned int *)GPH0CON)
#define rGPH0DAT	(*(volatile unsigned int *)GPH0DAT)
#define rGPH2CON	(*(volatile unsigned int *)GPH2CON)
#define rGPH2DAT	(*(volatile unsigned int *)GPH2DAT)

// 初始化按键
void key_init(void)
{
	// 设置GPHxCON寄存器，设置为输入模式
	// GPH0CON的bit8～15全部设置为0，即可
	rGPH0CON &= ~(0xFF<<8);
	// GPH2CON的bit0～15全部设置为0，即可
	rGPH2CON &= ~(0xFFFF<<0);
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
			// 为0，说明有按键
			led1();
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

























