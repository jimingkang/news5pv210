#include <linux/module.h>		// module_init  module_exit
#include <linux/init.h>			// __init   __exit
#include <linux/fs.h>
#include <linux/leds.h>
#include <mach/regs-gpio.h>
#include <mach/gpio-bank.h>
#include <linux/io.h>
#include <linux/ioport.h>
#include <mach/gpio.h>


#define GPIO_LED1	S5PV210_GPJ0(3)
#define GPIO_LED2	S5PV210_GPJ0(4)
#define GPIO_LED3	S5PV210_GPJ0(5)

#define X210_LED_OFF	1			// X210中LED是正极接电源，负极节GPIO
#define X210_LED_ON		0			// 所以1是灭，0是亮


static struct led_classdev mydev1;			// 定义结构体变量
static struct led_classdev mydev2;			// 定义结构体变量
static struct led_classdev mydev3;			// 定义结构体变量

// 这个函数就是要去完成具体的硬件读写任务的
static void s5pv210_led1_set(struct led_classdev *led_cdev,
			    enum led_brightness value)
{
	printk(KERN_INFO "s5pv210_led1_set\n");
	
	//writel(0x11111111, GPJ0CON);
	
	// 在这里根据用户设置的值来操作硬件
	// 用户设置的值就是value
	if (value == LED_OFF)
	{
		// 用户给了个0，希望LED灭
		//writel(0x11111111, GPJ0CON);
		// 读改写三部曲
		//writel((readl(GPJ0DAT) | (1<<3)), GPJ0DAT);
		gpio_set_value(GPIO_LED1, X210_LED_OFF);
	}
	else
	{
		// 用户给的是非0，希望LED亮
		//writel(0x11111111, GPJ0CON);
		//writel((readl(GPJ0DAT) & ~(1<<3)), GPJ0DAT);
		gpio_set_value(GPIO_LED1, X210_LED_ON);
	}
}

static void s5pv210_led2_set(struct led_classdev *led_cdev,
			    enum led_brightness value)
{
	printk(KERN_INFO "s5pv2102_led_set\n");
	
	//writel(0x11111111, GPJ0CON);
	
	// 在这里根据用户设置的值来操作硬件
	// 用户设置的值就是value
	if (value == LED_OFF)
	{
		// 用户给了个0，希望LED灭
		//writel(0x11111111, GPJ0CON);
		// 读改写三部曲
		//writel((readl(GPJ0DAT) | (1<<4)), GPJ0DAT);
	}
	else
	{
		// 用户给的是非0，希望LED亮
		//writel(0x11111111, GPJ0CON);
		//writel((readl(GPJ0DAT) & ~(1<<4)), GPJ0DAT);
	}
}

static void s5pv210_led3_set(struct led_classdev *led_cdev,
			    enum led_brightness value)
{
	printk(KERN_INFO "s5pv210_led3_set\n");
	
	//writel(0x11111111, GPJ0CON);
	
	// 在这里根据用户设置的值来操作硬件
	// 用户设置的值就是value
	if (value == LED_OFF)
	{
		// 用户给了个0，希望LED灭
		//writel(0x11111111, GPJ0CON);
		// 读改写三部曲
		//writel((readl(GPJ0DAT) | (1<<5)), GPJ0DAT);
	}
	else
	{
		// 用户给的是非0，希望LED亮
		//writel(0x11111111, GPJ0CON);
		//writel((readl(GPJ0DAT) & ~(1<<5)), GPJ0DAT);
	}
}


static int __init s5pv210_led_init(void)
{
	// 用户insmod安装驱动模块时会调用该函数
	// 该函数的主要任务就是去使用led驱动框架提供的设备注册函数来注册一个设备
	int ret = -1;
	
	// 在这里去申请驱动用到的各种资源，当前驱动中就是GPIO资源
	if (gpio_request(GPIO_LED1, "led1_gpj0.3")) 
	{
		printk(KERN_ERR "gpio_request failed\n");
	} 
	else 
	{
		// 设置为输出模式，并且默认输出1让LED灯灭
		gpio_direction_output(GPIO_LED1, 1);
	}
	
	
	
	// led1
	mydev1.name = "led1";
	mydev1.brightness = 0;	
	mydev1.brightness_set = s5pv210_led1_set;
	
	ret = led_classdev_register(NULL, &mydev1);
	if (ret < 0) {
		printk(KERN_ERR "led_classdev_register failed\n");
		return ret;
	}
	
	// led2
	mydev2.name = "led2";
	mydev2.brightness = 0;	
	mydev2.brightness_set = s5pv210_led2_set;
	
	ret = led_classdev_register(NULL, &mydev2);
	if (ret < 0) {
		printk(KERN_ERR "led_classdev_register failed\n");
		return ret;
	}
	
	// led3
	mydev3.name = "led3";
	mydev3.brightness = 0;	
	mydev3.brightness_set = s5pv210_led3_set;
	
	ret = led_classdev_register(NULL, &mydev3);
	if (ret < 0) {
		printk(KERN_ERR "led_classdev_register failed\n");
		return ret;
	}
	
	return 0;
}

static void __exit s5pv210_led_exit(void)
{
	led_classdev_unregister(&mydev1);
	led_classdev_unregister(&mydev2);
	led_classdev_unregister(&mydev3);
	
	gpio_free(GPIO_LED1);
}


module_init(s5pv210_led_init);
module_exit(s5pv210_led_exit);

// MODULE_xxx这种宏作用是用来添加模块描述信息
MODULE_LICENSE("GPL");							// 描述模块的许可证
MODULE_AUTHOR("aston <1264671872@qq.com>");		// 描述模块的作者
MODULE_DESCRIPTION("s5pv210 led driver");		// 描述模块的介绍信息
MODULE_ALIAS("s5pv210_led");					// 描述模块的别名信息








