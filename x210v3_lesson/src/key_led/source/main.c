/*
 * 实验名称：按键检测实验
 * 实验现象：插入SD卡并开机，按下LEFT，DOWN，RIGHT，BACK键时，对应D22，D23，D24，D25指示灯会亮，松开会灭
 * 输出映像：key_led.bin
 * 编          制：九鼎创展科技
 * 网	  址：http://www.9tripod.com
 * 论	  坛：http://bbs.9tripod.com
 * 日          期：2013-8-21
 */

#include <types.h>
#include <io.h>
#include <s5pv210/reg-gpio.h>
//#include <s5pv210/reg-serial.h>

static void __attribute__ ((noinline)) __delay(volatile u32_t loop)
{
	for(; loop > 0; loop--);
}

void mdelay(u32_t ms)
{
	__delay(ms * 1000);
}

/*

 */
void led_init(void)
{
	/*
	 * LED1 -> D22 -> GPJ0_3
	 * LED2 -> D23 -> GPJ0_4
	 * LED3 -> D24 -> GPJ0_5
	 * LED4 -> D25 -> GPD0_1
	 * */
	/* LED0 */
	writel(S5PV210_GPD0CON, (readl(S5PV210_GPD0CON) & ~(0xf<<4)) | (0x1<<4));
	writel(S5PV210_GPD0PUD, (readl(S5PV210_GPD0PUD) & ~(0x3<<2)) | (0x2<<2));
	writel(S5PV210_GPD0DAT, (readl(S5PV210_GPD0DAT) & ~(0x1<<1)) | (0x1<<1));
	/* LED1 */
	writel(S5PV210_GPJ0CON, (readl(S5PV210_GPJ0CON) & ~(0xf<<12)) | (0x1<<12));
	writel(S5PV210_GPJ0PUD, (readl(S5PV210_GPJ0PUD) & ~(0x3<<6)) | (0x2<<6));
	writel(S5PV210_GPJ0DAT, (readl(S5PV210_GPJ0DAT) & ~(0x1<<3)) | (0x1<<3));
	/* LED2 */
	writel(S5PV210_GPJ0CON, (readl(S5PV210_GPJ0CON) & ~(0xf<<16)) | (0x1<<16));
	writel(S5PV210_GPJ0PUD, (readl(S5PV210_GPJ0PUD) & ~(0x3<<8)) | (0x2<<8));
	writel(S5PV210_GPJ0DAT, (readl(S5PV210_GPJ0DAT) & ~(0x1<<4)) | (0x1<<4));
	/* LED3 */
	writel(S5PV210_GPJ0CON, (readl(S5PV210_GPJ0CON) & ~(0xf<<20)) | (0x1<<20));
	writel(S5PV210_GPJ0PUD, (readl(S5PV210_GPJ0PUD) & ~(0x3<<10)) | (0x2<<10));
	writel(S5PV210_GPJ0DAT, (readl(S5PV210_GPJ0DAT) & ~(0x1<<5)) | (0x1<<5));
}

void key_init(void)
{
	/*
	 * LEFT ->EINT2   -> GPH0_2
	 * DOWN ->EINT3   -> GPH0_3
	 * UP   ->KP_COL0 -> GPH2_0
	 * RIGHT->KP_COL1 -> GPH2_1
	 * BACK ->KP_COL2 -> GPH2_2
	 * MENU ->KP_COL3 -> GPH2_3
	 * */
	/* LEFT */
	writel(S5PV210_GPH2CON, (readl(S5PV210_GPH0CON) & ~(0xf<<8)) | (0x0<<8));
	writel(S5PV210_GPH2PUD, (readl(S5PV210_GPH0PUD) & ~(0x3<<4)) | (0x2<<4));
	/* DOWN */
	writel(S5PV210_GPH2CON, (readl(S5PV210_GPH0CON) & ~(0xf<<12)) | (0x0<<12));
	writel(S5PV210_GPH2PUD, (readl(S5PV210_GPH0PUD) & ~(0x3<<6)) | (0x2<<6));
	/* UP */
	writel(S5PV210_GPH2CON, (readl(S5PV210_GPH2CON) & ~(0xf<<0)) | (0x0<<0));
	writel(S5PV210_GPH2PUD, (readl(S5PV210_GPH2PUD) & ~(0x3<<0)) | (0x2<<0));
	/* RIGHT */
	writel(S5PV210_GPH2CON, (readl(S5PV210_GPH2CON) & ~(0xf<<4))| (0x0<<4));
	writel(S5PV210_GPH2PUD, (readl(S5PV210_GPH2PUD) & ~(0x3<<2)) | (0x2<<2));
	/* BACK */
	writel(S5PV210_GPH2CON, (readl(S5PV210_GPH2CON) & ~(0xf<<8))| (0x0<<8));
	writel(S5PV210_GPH2PUD, (readl(S5PV210_GPH2PUD) & ~(0x3<<4)) | (0x2<<4));
	/* MENU */
	writel(S5PV210_GPH2CON, (readl(S5PV210_GPH2CON) & ~(0xf<<12))| (0x0<<12));
	writel(S5PV210_GPH2PUD, (readl(S5PV210_GPH2PUD) & ~(0x3<<6)) | (0x2<<6));
}

void led_set(int id, int on)
{
	switch(id)
	{
	case 0:
		if(on)
			writel(S5PV210_GPJ0DAT, (readl(S5PV210_GPJ0DAT) & ~(0x1<<3)) | (0x0<<3));
		else
			writel(S5PV210_GPJ0DAT, (readl(S5PV210_GPJ0DAT) & ~(0x1<<3)) | (0x1<<3));
		break;

	case 1:
		if(on)
			writel(S5PV210_GPJ0DAT, (readl(S5PV210_GPJ0DAT) & ~(0x1<<4)) | (0x0<<4));
		else
			writel(S5PV210_GPJ0DAT, (readl(S5PV210_GPJ0DAT) & ~(0x1<<4)) | (0x1<<4));
		break;

	case 2:
		if(on)
			writel(S5PV210_GPJ0DAT, (readl(S5PV210_GPJ0DAT) & ~(0x1<<5)) | (0x0<<5));
		else
			writel(S5PV210_GPJ0DAT, (readl(S5PV210_GPJ0DAT) & ~(0x1<<5)) | (0x1<<5));
		break;

	case 3:
		if(on)
			writel(S5PV210_GPD0DAT, (readl(S5PV210_GPD0DAT) & ~(0x1<<1)) | (0x0<<1));
		else
			writel(S5PV210_GPD0DAT, (readl(S5PV210_GPD0DAT) & ~(0x1<<1)) | (0x1<<1));
		break;

	default:
		break;
	}
}

int main(int argc, char * argv[])
{
	led_init();
	key_init();
	while(1)
	{
		if(readl(S5PV210_GPH0DAT) & (0x1<<2))	//GPH0_2，LEFT
		{
			led_set(0,0);
		}
		else
		{
			led_set(0,1);
		}
		if(readl(S5PV210_GPH0DAT) & (0x1<<3))	//GPH0_3，DOWN
		{
			led_set(1,0);
		}
		else
		{
			led_set(1,1);
		}
		if(readl(S5PV210_GPH2DAT) & (0x1<<1))	//GPH2_1，RIGHT
		{
			led_set(2,0);
		}
		else
		{
			led_set(2,1);
		}
		if(readl(S5PV210_GPH2DAT) & (0x1<<2))	//GPH2_2，BACK
		{
			led_set(3,0);
		}
		else
		{
			led_set(3,1);
		}
	}
	return 0;
}
