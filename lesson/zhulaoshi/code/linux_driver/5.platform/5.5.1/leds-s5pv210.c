#include <linux/module.h>		// module_init  module_exit
#include <linux/init.h>			// __init   __exit
#include <linux/fs.h>
#include <linux/leds.h>
#include <mach/regs-gpio.h>
#include <mach/gpio-bank.h>
#include <linux/io.h>
#include <linux/ioport.h>
#include <mach/gpio.h>
#include <linux/platform_device.h>
#include <mach/leds-gpio.h>
#include <linux/slab.h>


#define X210_LED_OFF	1			// X210中LED是正极接电源，负极节GPIO
#define X210_LED_ON		0			// 所以1是灭，0是亮

struct s5pv210_gpio_led {
	struct led_classdev		 cdev;
	struct s5pv210_led_platdata	*pdata;
};

static inline struct s5pv210_gpio_led *pdev_to_gpio(struct platform_device *dev)
{
	return platform_get_drvdata(dev);
}

static inline struct s5pv210_gpio_led *to_gpio(struct led_classdev *led_cdev)
{
	return container_of(led_cdev, struct s5pv210_gpio_led, cdev);
}


// 这个函数就是要去完成具体的硬件读写任务的
static void s5pv210_led_set(struct led_classdev *led_cdev,
			    enum led_brightness value)
{
	struct s5pv210_gpio_led *p = to_gpio(led_cdev);
	
	printk(KERN_INFO "s5pv210_led_set\n");
	
	// 在这里根据用户设置的值来操作硬件
	// 用户设置的值就是value
	if (value == LED_OFF)
	{
		// 用户给了个0，希望LED灭
		gpio_set_value(p->pdata->gpio, X210_LED_OFF);
	}
	else
	{
		// 用户给的是非0，希望LED亮
		gpio_set_value(p->pdata->gpio, X210_LED_ON);
	}
}


static int s5pv210_led_probe(struct platform_device *dev)
{
	// 用户insmod安装驱动模块时会调用该函数
	// 该函数的主要任务就是去使用led驱动框架提供的设备注册函数来注册一个设备
	int ret = -1;
	struct s5pv210_led_platdata *pdata = dev->dev.platform_data;
	struct s5pv210_gpio_led *led;
	
	printk(KERN_INFO "----s5pv210_led_probe---\n");

	led = kzalloc(sizeof(struct s5pv210_gpio_led), GFP_KERNEL);
	if (led == NULL) {
		dev_err(&dev->dev, "No memory for device\n");
		return -ENOMEM;
	}

	platform_set_drvdata(dev, led);
	

	// 在这里去申请驱动用到的各种资源，当前驱动中就是GPIO资源
	if (gpio_request(pdata->gpio, pdata->name)) 
	{
		printk(KERN_ERR "gpio_request failed\n");
	} 
	else 
	{
		// 设置为输出模式，并且默认输出1让LED灯灭
		gpio_direction_output(pdata->gpio, 1);
	}
	
	// led1
	led->cdev.name = pdata->name;
	led->cdev.brightness = 0;	
	led->cdev.brightness_set = s5pv210_led_set;
	led->pdata = pdata;
	
	ret = led_classdev_register(&dev->dev, &led->cdev);
	if (ret < 0) {
		printk(KERN_ERR "led_classdev_register failed\n");
		return ret;
	}
	
	return 0;
}

static int s5pv210_led_remove(struct platform_device *dev)
{
	struct s5pv210_gpio_led *p = pdev_to_gpio(dev);

	led_classdev_unregister(&p->cdev);
	gpio_free(p->pdata->gpio);
	kfree(p);								// kfee放在最后一步
	
	return 0;
}



static struct platform_driver s5pv210_led_driver = {
	.probe		= s5pv210_led_probe,
	.remove		= s5pv210_led_remove,
	.driver		= {
		.name		= "s5pv210_led",
		.owner		= THIS_MODULE,
	},
};

static int __init s5pv210_led_init(void)
{
	return platform_driver_register(&s5pv210_led_driver);
}

static void __exit s5pv210_led_exit(void)
{
	platform_driver_unregister(&s5pv210_led_driver);
}


module_init(s5pv210_led_init);
module_exit(s5pv210_led_exit);

// MODULE_xxx这种宏作用是用来添加模块描述信息
MODULE_LICENSE("GPL");							// 描述模块的许可证
MODULE_AUTHOR("aston <1264671872@qq.com>");		// 描述模块的作者
MODULE_DESCRIPTION("s5pv210 led driver");		// 描述模块的介绍信息
MODULE_ALIAS("s5pv210_led");					// 描述模块的别名信息








