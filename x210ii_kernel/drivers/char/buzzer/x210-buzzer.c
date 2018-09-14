#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/poll.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <linux/interrupt.h>
#include <asm/uaccess.h>
#include <mach/hardware.h>
#include <plat/regs-timer.h>
#include <mach/regs-irq.h>
#include <asm/mach/time.h>
#include <linux/clk.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/miscdevice.h>

#include <linux/gpio.h>

#include <plat/gpio-cfg.h>
//#include <plat/regs-clock.h>
//#include <plat/regs-gpio.h>

//#include <plat/gpio-bank-e.h>
//#include <plat/gpio-bank-f.h>
//#include <plat/gpio-bank-k.h>

#define DEVICE_NAME     "buzzer"

#define PWM_IOCTL_SET_FREQ		1
#define PWM_IOCTL_STOP			0

static struct semaphore lock;

// TCFG0在Uboot中设置，这里不再重复设置
// Timer0输入频率Finput=pclk/(prescaler1+1)/MUX1
//                     =66M/16/16
// TCFG0 = tcnt = (pclk/16/16)/freq;
// PWM0输出频率Foutput =Finput/TCFG0= freq
static void PWM_Set_Freq( unsigned long freq )
{
	unsigned long tcon;
	unsigned long tcnt;
	unsigned long tcfg1;

	struct clk *clk_p;
	unsigned long pclk;

	//unsigned tmp;
	
	//设置GPD0_2为PWM输出
	s3c_gpio_cfgpin(S5PV210_GPD0(2), S3C_GPIO_SFN(2));

	tcon = __raw_readl(S3C2410_TCON);
	tcfg1 = __raw_readl(S3C2410_TCFG1);

	//mux = 1/16
	tcfg1 &= ~(0xf<<8);
	tcfg1 |= (0x4<<8);
	__raw_writel(tcfg1, S3C2410_TCFG1);
	
	clk_p = clk_get(NULL, "pclk");
	pclk  = clk_get_rate(clk_p);

	tcnt  = (pclk/16/16)/freq;
	
	__raw_writel(tcnt, S3C2410_TCNTB(2));
	__raw_writel(tcnt/2, S3C2410_TCMPB(2));//占空比为50%

	tcon &= ~(0xf<<12);
	tcon |= (0xb<<12);		//disable deadzone, auto-reload, inv-off, update TCNTB0&TCMPB0, start timer 0
	__raw_writel(tcon, S3C2410_TCON);
	
	tcon &= ~(2<<12);			//clear manual update bit
	__raw_writel(tcon, S3C2410_TCON);
}

void PWM_Stop( void )
{
	//将GPD0_2设置为input
	s3c_gpio_cfgpin(S5PV210_GPD0(2), S3C_GPIO_SFN(0));	
}

static int x210_pwm_open(struct inode *inode, struct file *file)
{
	if (!down_trylock(&lock))
		return 0;
	else
		return -EBUSY;
	
}


static int x210_pwm_close(struct inode *inode, struct file *file)
{
	up(&lock);
	return 0;
}

// PWM:GPF14->PWM0
static int x210_pwm_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
	switch (cmd) 
	{
		case PWM_IOCTL_SET_FREQ:
			printk("PWM_IOCTL_SET_FREQ:\r\n");
			if (arg == 0)
				return -EINVAL;
			PWM_Set_Freq(arg);
			break;

		case PWM_IOCTL_STOP:
		default:
			printk("PWM_IOCTL_STOP:\r\n");
			PWM_Stop();
			break;
	}

	return 0;
}


static struct file_operations dev_fops = {
    .owner   =   THIS_MODULE,
    .open    =   x210_pwm_open,
    .release =   x210_pwm_close, 
    .ioctl   =   x210_pwm_ioctl,
};

static struct miscdevice misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEVICE_NAME,
	.fops = &dev_fops,
};

static int __init dev_init(void)
{
	int ret;

	init_MUTEX(&lock);
	ret = misc_register(&misc);
	
	/* GPD0_2 (PWMTOUT2) */
	ret = gpio_request(S5PV210_GPD0(2), "GPD0");
	if(ret)
		printk("buzzer-x210: request gpio GPD0(2) fail");
		
	s3c_gpio_setpull(S5PV210_GPD0(2), S3C_GPIO_PULL_UP);
	s3c_gpio_cfgpin(S5PV210_GPD0(2), S3C_GPIO_SFN(1));
	gpio_set_value(S5PV210_GPD0(2), 0);

	printk ("x210 "DEVICE_NAME" initialized\n");
    	return ret;
}

static void __exit dev_exit(void)
{
	misc_deregister(&misc);
}

module_init(dev_init);
module_exit(dev_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("www.9tripod.com");
MODULE_DESCRIPTION("x210 PWM Driver");
