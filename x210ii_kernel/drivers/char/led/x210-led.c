/*
 * (C) 2011-2013 by xboot.org
 * Author: jianjun jiang <jerryjianjun@gmail.com>
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation
 *
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <mach/hardware.h>
#include <asm/mach-types.h>
#include <linux/gpio.h>
#include <asm/gpio.h>
#include <asm/delay.h>
#include <linux/clk.h>
#include <plat/gpio-cfg.h>

/*
 * X210:
 *
 * LED1 -> D22 -> GPJ0_3
 * LED2 -> D23 -> GPJ0_4
 * LED3 -> D24 -> GPJ0_5
 * LED4 -> D25 -> GPD0_1
 */
/*
 * I210:
 *
 * LED1 -> D22 -> GPJ2_0
 * LED2 -> D23 -> GPJ2_1
 * LED3 -> D24 -> GPJ2_2
 * LED4 -> D25 -> GPJ2_3
 */
#define CONFIG_X210			(1)
//#define CONFIG_I210		(1)

#if defined(CONFIG_X210) && (CONFIG_X210 > 0)
static int __x210_led_status[4] = { 0 };

static void __x210_led_probe(void)
{
	int ret;

	ret = gpio_request(S5PV210_GPJ0(3), "GPJ0");
	if(ret)
		printk("x210-led: request gpio GPJ0(3) fail\n");
	s3c_gpio_setpull(S5PV210_GPJ0(3), S3C_GPIO_PULL_UP);
	s3c_gpio_cfgpin(S5PV210_GPJ0(3), S3C_GPIO_SFN(1));
	gpio_set_value(S5PV210_GPJ0(3), 1);

	ret = gpio_request(S5PV210_GPJ0(4), "GPJ0");
	if(ret)
		printk("x210-led: request gpio GPJ0(4) fail\n");
	s3c_gpio_setpull(S5PV210_GPJ0(4), S3C_GPIO_PULL_UP);
	s3c_gpio_cfgpin(S5PV210_GPJ0(4), S3C_GPIO_SFN(1));
	gpio_set_value(S5PV210_GPJ0(4), 1);

	ret = gpio_request(S5PV210_GPJ0(5), "GPJ0");
	if(ret)
		printk("x210-led: request gpio GPJ0(5) fail\n");
	s3c_gpio_setpull(S5PV210_GPJ0(5), S3C_GPIO_PULL_UP);
	s3c_gpio_cfgpin(S5PV210_GPJ0(5), S3C_GPIO_SFN(1));
	gpio_set_value(S5PV210_GPJ0(5), 1);

	ret = gpio_request(S5PV210_GPD0(1), "GPD0");
	if(ret)
		printk("x210-led: request gpio GPD0(1) fail\n");
	s3c_gpio_setpull(S5PV210_GPD0(1), S3C_GPIO_PULL_UP);
	s3c_gpio_cfgpin(S5PV210_GPD0(1), S3C_GPIO_SFN(1));
	gpio_set_value(S5PV210_GPD0(1), 1);

	__x210_led_status[0] = 0;
	__x210_led_status[1] = 0;
	__x210_led_status[2] = 0;
	__x210_led_status[3] = 0;
}

static void __x210_led_remove(void)
{
	gpio_free(S5PV210_GPJ0(3));
	gpio_free(S5PV210_GPJ0(4));
	gpio_free(S5PV210_GPJ0(5));
	gpio_free(S5PV210_GPD0(1));
}

static ssize_t x210_led_read(struct device *dev, struct device_attribute *attr, char *buf)
{
	if(!strcmp(attr->attr.name, "led1"))
	{
		if(__x210_led_status[0] != 0)
			return strlcpy(buf, "1\n", 3);
		else
			return strlcpy(buf, "0\n", 3);
	}
	else if(!strcmp(attr->attr.name, "led2"))
	{
		if(__x210_led_status[1] != 0)
			return strlcpy(buf, "1\n", 3);
		else
			return strlcpy(buf, "0\n", 3);
	}
	else if(!strcmp(attr->attr.name, "led3"))
	{
		if(__x210_led_status[2] != 0)
			return strlcpy(buf, "1\n", 3);
		else
			return strlcpy(buf, "0\n", 3);
	}
	else if(!strcmp(attr->attr.name, "led4"))
	{
		if(__x210_led_status[3] != 0)
			return strlcpy(buf, "1\n", 3);
		else
			return strlcpy(buf, "0\n", 3);
	}
}

static ssize_t x210_led_write(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	unsigned long on = simple_strtoul(buf, NULL, 10);

	if(!strcmp(attr->attr.name, "led1"))
	{
		if(on)
		{
			gpio_set_value(S5PV210_GPJ0(3), 0);
			__x210_led_status[0] = 1;
		}
		else
		{
			gpio_set_value(S5PV210_GPJ0(3), 1);
			__x210_led_status[0] = 0;
		}
	}
	else if(!strcmp(attr->attr.name, "led2"))
	{
		if(on)
		{
			gpio_set_value(S5PV210_GPJ0(4), 0);
			__x210_led_status[1] = 1;
		}
		else
		{
			gpio_set_value(S5PV210_GPJ0(4), 1);
			__x210_led_status[1] = 0;
		}
	}
	else if(!strcmp(attr->attr.name, "led3"))
	{
		if(on)
		{
			gpio_set_value(S5PV210_GPJ0(5), 0);
			__x210_led_status[2] = 1;
		}
		else
		{
			gpio_set_value(S5PV210_GPJ0(5), 1);
			__x210_led_status[2] = 0;
		}
	}
	else if(!strcmp(attr->attr.name, "led4"))
	{
		if(on)
		{
			gpio_set_value(S5PV210_GPD0(1), 0);
			__x210_led_status[3] = 1;
		}
		else
		{
			gpio_set_value(S5PV210_GPD0(1), 1);
			__x210_led_status[3] = 0;
		}
	}

	return count;
}

static DEVICE_ATTR(led1, 0666, x210_led_read, x210_led_write);
static DEVICE_ATTR(led2, 0666, x210_led_read, x210_led_write);
static DEVICE_ATTR(led3, 0666, x210_led_read, x210_led_write);
static DEVICE_ATTR(led4, 0666, x210_led_read, x210_led_write);

static struct attribute * x210_led_sysfs_entries[] = {
	&dev_attr_led1.attr,
	&dev_attr_led2.attr,
	&dev_attr_led3.attr,
	&dev_attr_led4.attr,
	NULL,
};
#elif defined(CONFIG_I210) && (CONFIG_I210 > 0)
static int __x210_led_status[4] = { 0 };

static void __x210_led_probe(void)
{
	int ret;

	ret = gpio_request(S5PV210_GPJ2(0), "GPJ2");
	if(ret)
		printk("x210-led: request gpio GPJ2(0) fail\n");
	s3c_gpio_setpull(S5PV210_GPJ2(0), S3C_GPIO_PULL_UP);
	s3c_gpio_cfgpin(S5PV210_GPJ2(0), S3C_GPIO_SFN(1));
	gpio_set_value(S5PV210_GPJ2(0), 1);

	ret = gpio_request(S5PV210_GPJ2(1), "GPJ2");
	if(ret)
		printk("x210-led: request gpio GPJ2(1) fail\n");
	s3c_gpio_setpull(S5PV210_GPJ2(1), S3C_GPIO_PULL_UP);
	s3c_gpio_cfgpin(S5PV210_GPJ2(1), S3C_GPIO_SFN(1));
	gpio_set_value(S5PV210_GPJ2(1), 1);

	ret = gpio_request(S5PV210_GPJ2(2), "GPJ2");
	if(ret)
		printk("x210-led: request gpio GPJ2(2) fail\n");
	s3c_gpio_setpull(S5PV210_GPJ2(2), S3C_GPIO_PULL_UP);
	s3c_gpio_cfgpin(S5PV210_GPJ2(2), S3C_GPIO_SFN(1));
	gpio_set_value(S5PV210_GPJ2(2), 1);

	ret = gpio_request(S5PV210_GPJ2(3), "GPJ2");
	if(ret)
		printk("x210-led: request gpio GPJ2(3) fail\n");
	s3c_gpio_setpull(S5PV210_GPJ2(3), S3C_GPIO_PULL_UP);
	s3c_gpio_cfgpin(S5PV210_GPJ2(3), S3C_GPIO_SFN(1));
	gpio_set_value(S5PV210_GPJ2(3), 1);

	__x210_led_status[0] = 0;
	__x210_led_status[1] = 0;
	__x210_led_status[2] = 0;
	__x210_led_status[3] = 0;
}

static void __x210_led_remove(void)
{
	gpio_free(S5PV210_GPJ2(0));
	gpio_free(S5PV210_GPJ2(1));
	gpio_free(S5PV210_GPJ2(2));
	gpio_free(S5PV210_GPJ2(3));
}

static ssize_t x210_led_read(struct device *dev, struct device_attribute *attr, char *buf)
{
	if(!strcmp(attr->attr.name, "led1"))
	{
		if(__x210_led_status[0] != 0)
			return strlcpy(buf, "1\n", 3);
		else
			return strlcpy(buf, "0\n", 3);
	}
	else if(!strcmp(attr->attr.name, "led2"))
	{
		if(__x210_led_status[1] != 0)
			return strlcpy(buf, "1\n", 3);
		else
			return strlcpy(buf, "0\n", 3);
	}
	else if(!strcmp(attr->attr.name, "led3"))
	{
		if(__x210_led_status[2] != 0)
			return strlcpy(buf, "1\n", 3);
		else
			return strlcpy(buf, "0\n", 3);
	}
	else if(!strcmp(attr->attr.name, "led4"))
	{
		if(__x210_led_status[3] != 0)
			return strlcpy(buf, "1\n", 3);
		else
			return strlcpy(buf, "0\n", 3);
	}
}

static ssize_t x210_led_write(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	unsigned long on = simple_strtoul(buf, NULL, 10);

	if(!strcmp(attr->attr.name, "led1"))
	{
		if(on)
		{
			gpio_set_value(S5PV210_GPJ2(0), 0);
			__x210_led_status[0] = 1;
		}
		else
		{
			gpio_set_value(S5PV210_GPJ2(0), 1);
			__x210_led_status[0] = 0;
		}
	}
	else if(!strcmp(attr->attr.name, "led2"))
	{
		if(on)
		{
			gpio_set_value(S5PV210_GPJ2(1), 0);
			__x210_led_status[1] = 1;
		}
		else
		{
			gpio_set_value(S5PV210_GPJ2(1), 1);
			__x210_led_status[1] = 0;
		}
	}
	else if(!strcmp(attr->attr.name, "led3"))
	{
		if(on)
		{
			gpio_set_value(S5PV210_GPJ2(2), 0);
			__x210_led_status[2] = 1;
		}
		else
		{
			gpio_set_value(S5PV210_GPJ2(2), 1);
			__x210_led_status[2] = 0;
		}
	}
	else if(!strcmp(attr->attr.name, "led4"))
	{
		if(on)
		{
			gpio_set_value(S5PV210_GPJ2(3), 0);
			__x210_led_status[3] = 1;
		}
		else
		{
			gpio_set_value(S5PV210_GPJ2(3), 1);
			__x210_led_status[3] = 0;
		}
	}

	return count;
}

static DEVICE_ATTR(led1, 0666, x210_led_read, x210_led_write);
static DEVICE_ATTR(led2, 0666, x210_led_read, x210_led_write);
static DEVICE_ATTR(led3, 0666, x210_led_read, x210_led_write);
static DEVICE_ATTR(led4, 0666, x210_led_read, x210_led_write);

static struct attribute * x210_led_sysfs_entries[] = {
	&dev_attr_led1.attr,
	&dev_attr_led2.attr,
	&dev_attr_led3.attr,
	&dev_attr_led4.attr,
	NULL,
};
#endif

static struct attribute_group x210_led_attr_group = {
	.name	= NULL,
	.attrs	= x210_led_sysfs_entries,
};

static int x210_led_probe(struct platform_device *pdev)
{
	__x210_led_probe();

	return sysfs_create_group(&pdev->dev.kobj, &x210_led_attr_group);
}

static int x210_led_remove(struct platform_device *pdev)
{
	__x210_led_remove();

	sysfs_remove_group(&pdev->dev.kobj, &x210_led_attr_group);
	return 0;
}

#ifdef CONFIG_PM
static int x210_led_suspend(struct platform_device *pdev, pm_message_t state)
{
	return 0;
}

static int x210_led_resume(struct platform_device *pdev)
{
	return 0;
}

#else
#define x210_led_suspend	NULL
#define x210_led_resume	NULL
#endif

static struct platform_driver x210_led_driver = {
	.probe		= x210_led_probe,
	.remove		= x210_led_remove,
	.suspend	= x210_led_suspend,
	.resume		= x210_led_resume,
	.driver		= {
		.name	= "x210-led",
	},
};

static struct platform_device x210_led_device = {
	.name      = "x210-led",
	.id        = -1,
};

static int __devinit x210_led_init(void)
{
	int ret;

	printk("x210 led driver\r\n");

	ret = platform_device_register(&x210_led_device);
	if(ret)
		printk("failed to register x210 led device\n");

	ret = platform_driver_register(&x210_led_driver);
	if(ret)
		printk("failed to register x210 led driver\n");

	return ret;
}

static void x210_led_exit(void)
{
	platform_driver_unregister(&x210_led_driver);
}

module_init(x210_led_init);
module_exit(x210_led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("jianjun jiang <jerryjianjun@gmail.com>");
MODULE_DESCRIPTION("x210 led driver");
