/*
 * 实验名称：蜂鸣器实验
 * 实验现象：插入SD卡并开机，蜂鸣器每隔约500ms响一次
 * 输出映像：buzzer.bin
 * 编          制：九鼎创展科技
 * 网	  址：http://www.9tripod.com
 * 论	  坛：http://bbs.9tripod.com
 * 日          期：2013-8-21
 */

#include <types.h>
#include <io.h>
#include <s5pv210/reg-gpio.h>

/*
 * 延时函数
 */
static void __attribute__ ((noinline)) __delay(volatile u32_t loop)
{
	for(; loop > 0; loop--);
}

void mdelay(u32_t ms)
{
	__delay(ms * 1000);
}

/*
 * speader enable: GPD0_2
 * 初始化GPIO口
 */
void speaker_init(void)
{
	writel(S5PV210_GPD0CON, (readl(S5PV210_GPD0CON) & ~(0xf<<8)) | (0x1<<8));
	//writel(S5PV210_GPD0PUD, (readl(S5PV210_GPD0PUD) & ~(0x3<<4)) | (0x2<<4));
	writel(S5PV210_GPD0DAT, (readl(S5PV210_GPD0DAT) & ~(0x1<<2)) | (0x0<<2));
}

/*
 * 蜂鸣器使能函数，on=1：蜂鸣  on=0：停止蜂鸣
 */
void speaker_set(int on)
{
	if(on)
		writel(S5PV210_GPD0DAT, (readl(S5PV210_GPD0DAT)&~(0x1<<2))|(0x1<<2));
	else
		writel(S5PV210_GPD0DAT, (readl(S5PV210_GPD0DAT)&~(0x1<<2))|(0x0<<2));
}

int main(int argc, char * argv[])
{
	speaker_init();
	while(1)
	{
		speaker_set(1);
		mdelay(1000);
		speaker_set(0);
		mdelay(1000);
	}
	return 0;
}
