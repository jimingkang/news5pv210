/*
 * ʵ�����ƣ�������ʵ��
 * ʵ�����󣺲���SD����������������ÿ��Լ500ms��һ��
 * ���ӳ��buzzer.bin
 * ��          �ƣ��Ŷ���չ�Ƽ�
 * ��	  ַ��http://www.9tripod.com
 * ��	  ̳��http://bbs.9tripod.com
 * ��          �ڣ�2013-8-21
 */

#include <types.h>
#include <io.h>
#include <s5pv210/reg-gpio.h>

/*
 * ��ʱ����
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
 * ��ʼ��GPIO��
 */
void speaker_init(void)
{
	writel(S5PV210_GPD0CON, (readl(S5PV210_GPD0CON) & ~(0xf<<8)) | (0x1<<8));
	//writel(S5PV210_GPD0PUD, (readl(S5PV210_GPD0PUD) & ~(0x3<<4)) | (0x2<<4));
	writel(S5PV210_GPD0DAT, (readl(S5PV210_GPD0DAT) & ~(0x1<<2)) | (0x0<<2));
}

/*
 * ������ʹ�ܺ�����on=1������  on=0��ֹͣ����
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
