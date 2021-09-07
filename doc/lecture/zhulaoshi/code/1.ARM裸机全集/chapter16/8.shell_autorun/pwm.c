
#define 	GPD0CON		(0xE02000A0)
#define 	TCFG0		(0xE2500000)
#define 	TCFG1		(0xE2500004)
#define 	CON			(0xE2500008)
#define 	TCNTB2		(0xE2500024)
#define 	TCMPB2		(0xE2500028)

#define 	rGPD0CON	(*(volatile unsigned int *)GPD0CON)
#define 	rTCFG0		(*(volatile unsigned int *)TCFG0)
#define 	rTCFG1		(*(volatile unsigned int *)TCFG1)
#define 	rCON		(*(volatile unsigned int *)CON)
#define 	rTCNTB2		(*(volatile unsigned int *)TCNTB2)
#define 	rTCMPB2		(*(volatile unsigned int *)TCMPB2)

#define 	GPD0DAT		(0xE02000A4)
#define 	rGPD0DAT	(*(volatile unsigned int *)GPD0DAT)



/*
// 初始化PWM timer2，使其输出PWM波形：频率是2KHz、duty为50%
void timer2_pwm_init(void)
{
	// 设置GPD0_2引脚，将其配置为XpwmTOUT_2
	rGPD0CON &= ~(0xf<<8);
	rGPD0CON |= (2<<8);
	
	// 设置PWM定时器的一干寄存器，使其工作
	rTCFG0 &= ~(0xff<<8);
	rTCFG0 |= (65<<8);			// prescaler1 = 65, 预分频后频率为1MHz
	
	rTCFG1 &= ~(0x0f<<8);
	rTCFG1 |= (1<<8);			// MUX2设置为1/2,分频后时钟周期为500KHz
	// 时钟设置好，我们的时钟频率是500KHz，对应的时钟周期是2us。也就是说每隔2us
	// 计一次数。如果要定的时间是x，则TCNTB中应该写入x/2us
	
	rCON |= (1<<15);		// 使能auto-reload，反复定时才能发出PWM波形
	//rTCNTB2 = 250;			// 0.5ms/2us = 500us/2us = 250
	//rTCMPB2 = 125;			// duty = 50%
	
	rTCNTB2 = 50;			
	rTCMPB2 = 25;	
	
	// 第一次需要手工将TCNTB中的值刷新到TCNT中去，以后就可以auto-reload了
	rCON |= (1<<13);		// 打开自动刷新功能
	rCON &= ~(1<<13);		// 关闭自动刷新功能
	
	rCON |= (1<<12);		// 开timer2定时器。要先把其他都设置好才能开定时器
}

*/

/*
void buzzer_on(void)
{
	rGPD0CON &= ~(0xf<<8);
	rGPD0CON |= (2<<8);
}

// 关蜂鸣器的思路：TCON中设置关，GPIO设置成其他模式
void buzzer_off(void)
{
	rGPD0CON &= ~(0xf<<8);
	rGPD0CON |= (0<<8);
}
*/

// 现象：引脚直接配置成输出模式，然后输出高电平，buzzer叫了。输出低电平就关了
// 优点就是SOC不用有pwm功能，只要能输出高低电平就能控制蜂鸣器
// 缺点是蜂鸣器的鸣叫的频率无法改动
// 如果可以，那么开关蜂鸣器就变成了GPIO输出高低电平了。

/*
// 测试结论：不能关闭蜂鸣器、但是打开和关闭自动装载前后，蜂鸣器的音色会变
// 应该是影响了输出PWM波形的频率导致的
void buzzer_on(void)
{
	rCON |= (1<<13);		// 打开自动刷新功能
}

void buzzer_off(void)
{
	rCON &= ~(1<<13);		// 关闭自动刷新功能
}
*/

/*
// 测试结论：加上蜂鸣器开关控制后，on和off操作对蜂鸣器毫无影响了，连音色也不变了
void buzzer_on(void)
{
	rCON |= (1<<13);		// 打开自动刷新功能
	rCON |= (1<<12);
}

void buzzer_off(void)
{
	rCON &= ~(1<<13);		// 关闭自动刷新功能
	rCON &= ~(1<<12);
}
*/


// 测试结论：引脚直接设置成输出模式确实能驱动蜂鸣器。输出高电平蜂鸣器响，低电平蜂鸣
// 器不响。
void timer2_pwm_init(void)
{
	rGPD0CON &= ~(0xf<<8);
	rGPD0CON |= (1<<8);			// 设置成output模式
	rGPD0DAT |= (1<<2);			// 输出高电平
}

void buzzer_on(void)
{
	rGPD0DAT |= (1<<2);	
}

void buzzer_off(void)
{
	rGPD0DAT &= ~(1<<2);	
}














