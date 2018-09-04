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
 * LED1 -> D22 -> GPJ0_3
 * LED2 -> D23 -> GPJ0_4
 * LED3 -> D24 -> GPJ0_5
 * LED4 -> D25 -> GPD0_1
 */
void led_init(void)
{
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

	/* LED4 */
	writel(S5PV210_GPD0CON, (readl(S5PV210_GPD0CON) & ~(0xf<<4)) | (0x1<<4));
	writel(S5PV210_GPD0PUD, (readl(S5PV210_GPD0PUD) & ~(0x3<<2)) | (0x2<<2));
	writel(S5PV210_GPD0DAT, (readl(S5PV210_GPD0DAT) & ~(0x1<<1)) | (0x1<<1));
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
	int index = 0;
	int flag = 0;

	led_init();

	while(1)
	{
		led_set(0, 0);
		led_set(1, 0);
		led_set(2, 0);
		led_set(3, 0);

		led_set(index, 1);
		mdelay(200);

		if(index == 0 || index == 3)
			flag = !flag;

		if(flag)
			index = (index + 1) % 4;
		else
			index = (index + 4 - 1) % 4;
	}

	return 0;
}
