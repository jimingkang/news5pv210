/*
 * WIFI Power Management
 *
 * (C) 2007-2009 by smit Inc.
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
//#include <mach/gpio-bank-b.h>

static int power_on_status = 0;

static void pm_delay(unsigned int ms)
{
	mdelay(ms);
}

static void pm_wifi_reset_pin(int level)
{
	if(level)
	{

	}
	else
	{

	}
}

static void pm_wifi_power_pin(int level)
{
	if(level)
	{
		gpio_set_value(S5PV210_GPH2(6), 0);	
		s3c_gpio_setpull(S5PV210_GPH2(6), S3C_GPIO_PULL_DOWN);
		s3c_gpio_cfgpin(S5PV210_GPH2(6), S3C_GPIO_SFN(1));
	}
	else
	{
		gpio_set_value(S5PV210_GPH2(6), 1);	
		s3c_gpio_setpull(S5PV210_GPH2(6), S3C_GPIO_PULL_UP);
		s3c_gpio_cfgpin(S5PV210_GPH2(6), S3C_GPIO_SFN(1));
	}
}

static void pm_wifi_powerdown_pin(int level)
{
	if(level)
	{
		gpio_set_value(S5PV210_GPH2(7), 1);	
		s3c_gpio_setpull(S5PV210_GPH2(7), S3C_GPIO_PULL_UP);
		s3c_gpio_cfgpin(S5PV210_GPH2(7), S3C_GPIO_SFN(1));
	}
	else
	{
		gpio_set_value(S5PV210_GPH2(7), 0);	
		s3c_gpio_setpull(S5PV210_GPH2(7), S3C_GPIO_PULL_DOWN);
		s3c_gpio_cfgpin(S5PV210_GPH2(7), S3C_GPIO_SFN(1));
	}
}

void pm_wifi_enable(void)
{
	pm_wifi_power_pin(1);
	pm_delay(10);
	
	pm_wifi_powerdown_pin(1);
	pm_delay(10);
	pm_wifi_powerdown_pin(0);
	pm_delay(10);
	pm_wifi_powerdown_pin(1);
	pm_delay(10);

	pm_wifi_reset_pin(1);
	pm_delay(10);
	pm_wifi_reset_pin(0);
	pm_delay(10);
	pm_wifi_reset_pin(1);
	pm_delay(10);

	/* mmc1 cd pin to low level for plugin in */
	s3c_gpio_setpull(S5PV210_GPG1(2), S3C_GPIO_PULL_DOWN);

	/* power on status */
	power_on_status = 1;
	
	printk("wifi power enabled\n");
}
EXPORT_SYMBOL(pm_wifi_enable);

void pm_wifi_disable(void)
{
	/* mmc1 cd pin to high level for plugin out */
	s3c_gpio_setpull(S5PV210_GPG1(2), S3C_GPIO_PULL_UP);

	pm_wifi_powerdown_pin(0);
	pm_delay(10);
	pm_wifi_power_pin(0);
	pm_delay(10);

	/* power on status */
	power_on_status = 0;

	printk("wifi power disabled\n");
}
EXPORT_SYMBOL(pm_wifi_disable);

static ssize_t pm_wifi_read(struct device *dev, struct device_attribute *attr, char *buf)
{
	int ret = 0;

	if (!strcmp(attr->attr.name, "power_on"))
	{
		ret = power_on_status;
	}

	if(ret)
		return strlcpy(buf, "1\n", 3);
	else
		return strlcpy(buf, "0\n", 3);
}

static ssize_t pm_wifi_write(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	unsigned long on = simple_strtoul(buf, NULL, 10);

	if(!strcmp(attr->attr.name, "power_on"))
	{
		if(on)
		{
			pm_wifi_enable();
		}
		else
		{
			pm_wifi_disable();
		}
	}
	
	return count;
}

static DEVICE_ATTR(power_on, 0666, pm_wifi_read, pm_wifi_write);

static struct attribute * pm_wifi_sysfs_entries[] = {
	&dev_attr_power_on.attr,
	NULL,
};

static struct attribute_group pm_wifi_attr_group = {
	.name	= NULL,
	.attrs	= pm_wifi_sysfs_entries,
};

static int pm_wifi_probe(struct platform_device *pdev)
{
	int ret;

	ret = gpio_request(S5PV210_GPH2(6), "GPH2");
	if(ret)
		printk("request gpio (gph2_6) fail\n");

	ret = gpio_request(S5PV210_GPH2(7), "GPH3");
	if(ret)
		printk("request gpio (gph2_7) fail\n");

	return sysfs_create_group(&pdev->dev.kobj, &pm_wifi_attr_group);
}

static int pm_wifi_remove(struct platform_device *pdev)
{
	gpio_free(S5PV210_GPH2(6));
	gpio_free(S5PV210_GPH2(7));

	sysfs_remove_group(&pdev->dev.kobj, &pm_wifi_attr_group);
	return 0;
}


#ifdef CONFIG_PM
static int pm_wifi_suspend(struct platform_device *pdev, pm_message_t state)
{
	if(power_on_status == 0)
	{
		pm_wifi_disable();
	}

	return 0;
}

static int pm_wifi_resume(struct platform_device *pdev)
{
	if(power_on_status)
	{
		pm_wifi_enable();
	}

	return 0;
}

#else
#define pm_wifi_suspend	NULL
#define pm_wifi_resume	NULL
#endif

static struct platform_driver pm_wifi_driver = {
	.probe		= pm_wifi_probe,
	.remove		= pm_wifi_remove,
	.suspend	= pm_wifi_suspend,
	.resume		= pm_wifi_resume,
	.driver		= {
		.name	= "pm-wifi",
	},
};

static struct platform_device pm_wifi_device = {
	.name      = "pm-wifi",
	.id        = -1,
};

static int __devinit pm_wifi_init(void)
{
	int ret;

	printk("wifi power management\r\n");

	ret = platform_device_register(&pm_wifi_device);
	if(ret)
		printk("failed to register wifi power device\n");

	ret = platform_driver_register(&pm_wifi_driver);
	if(ret)
		printk("failed to register wifi power driver\n");

	return ret;
}

static void pm_wifi_exit(void)
{
	platform_driver_unregister(&pm_wifi_driver);
}

module_init(pm_wifi_init);
module_exit(pm_wifi_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("jianjun jiang <jerryjianjun@gmail.com>");
MODULE_DESCRIPTION("wifi Power Management");

