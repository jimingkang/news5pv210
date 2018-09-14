/*
 * Driver for keys on GPIO lines.
 *
 * Copyright 2009 jiangjianjun <jerryjianjun@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/ioport.h>
#include <linux/delay.h>
#include <linux/serial_core.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <plat/map-base.h>
#include <plat/gpio-cfg.h>
#include <mach/regs-gpio.h>
#include <mach/regs-irq.h>
#include <linux/gpio.h>
#include <linux/input.h>

#define MAX_BUTTON_CNT 		(7)

static struct timer_list timer;
static struct input_dev * input;

static int s3c_Keycode[MAX_BUTTON_CNT] = {KEY_POWER, KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_MENU, KEY_BACK};
static int s3c_button_history[MAX_BUTTON_CNT] = { 0 };

static void s3cbutton_timer_handler(unsigned long data)
{
	int flag;

	/* power */
	flag = gpio_get_value(S5PV210_GPH0(1));
	if(flag != s3c_button_history[0])
	{
		if(flag)
			input_report_key(input, s3c_Keycode[0], 0);
		else
			input_report_key(input, s3c_Keycode[0], 1);
		s3c_button_history[0] = flag;
	}

	/* up */
	flag = gpio_get_value(S5PV210_GPH0(4));
	if(flag != s3c_button_history[1])
	{
		if(flag)
			input_report_key(input, s3c_Keycode[1], 0);
		else
			input_report_key(input, s3c_Keycode[1], 1);
		s3c_button_history[1] = flag;
	}

	/* down */
	flag = gpio_get_value(S5PV210_GPH3(5));
	if(flag != s3c_button_history[2])
	{
		if(flag)
			input_report_key(input, s3c_Keycode[2], 0);
		else
			input_report_key(input, s3c_Keycode[2], 1);
		s3c_button_history[2] = flag;
	}

	/* left */
	flag = gpio_get_value(S5PV210_GPH3(4));
	if(flag != s3c_button_history[3])
	{
		if(flag)
			input_report_key(input, s3c_Keycode[3], 0);
		else
			input_report_key(input, s3c_Keycode[3], 1);
		s3c_button_history[3] = flag;
	}

	/* right */
	flag = gpio_get_value(S5PV210_GPH0(7));
	if(flag != s3c_button_history[4])
	{
		if(flag)
			input_report_key(input, s3c_Keycode[4], 0);
		else
			input_report_key(input, s3c_Keycode[4], 1);
		s3c_button_history[4] = flag;
	}

	/* menu */
	flag = gpio_get_value(S5PV210_GPH2(0));
	if(flag != s3c_button_history[5])
	{
		if(flag)
			input_report_key(input, s3c_Keycode[5], 0);
		else
			input_report_key(input, s3c_Keycode[5], 1);
		s3c_button_history[5] = flag;
	}

	/* back */
	flag = gpio_get_value(S5PV210_GPH1(1));
	if(flag != s3c_button_history[6])
	{
		if(flag)
			input_report_key(input, s3c_Keycode[6], 0);
		else
			input_report_key(input, s3c_Keycode[6], 1);
		s3c_button_history[6] = flag;
	}

	/* Kernel Timer restart */
	mod_timer(&timer,jiffies + HZ/100);
}

static int s3c_button_probe(struct platform_device *pdev)
{
	int i;
	int ret;
	
	/* gph0_1 (power) */
	ret = gpio_request(S5PV210_GPH0(1), "GPH0");
	if(ret)
		printk("button-smdkv210: request gpio GPH0(1) fail");

	/* gph0_4 (up) */
	ret = gpio_request(S5PV210_GPH0(4), "GPH3");
	if(ret)
		printk("button-smdkv210: request gpio GPH0(4) fail");

	/* gph3_5 (down) */
	ret = gpio_request(S5PV210_GPH3(5), "GPH3");
	if(ret)
		printk("button-smdkv210: request gpio GPH3(5) fail");

	/* gph3_4 (left) */
	ret = gpio_request(S5PV210_GPH3(4), "GPH3");
	if(ret)
		printk("button-smdkv210: request gpio GPH3(4) fail");

	/* gph0_7 (right) */
	ret = gpio_request(S5PV210_GPH0(7), "GPH0");
	if(ret)
		printk("button-smdkv210: request gpio GPH0(7) fail");

	/* gph2_0 (menu) */
	ret = gpio_request(S5PV210_GPH2(0), "GPH2");
	if(ret)
		printk("button-smdkv210: request gpio GPH2(0) fail");

	/* gph1_1 (back) */
	ret = gpio_request(S5PV210_GPH1(1), "GPH1");
	if(ret)
		printk("button-smdkv210: request gpio GPH1(1) fail");

	s3c_gpio_setpull(S5PV210_GPH0(1), S3C_GPIO_PULL_UP);
	s3c_gpio_cfgpin(S5PV210_GPH0(1), S3C_GPIO_SFN(0));

	s3c_gpio_setpull(S5PV210_GPH0(4), S3C_GPIO_PULL_UP);
	s3c_gpio_cfgpin(S5PV210_GPH0(4), S3C_GPIO_SFN(0));

	s3c_gpio_setpull(S5PV210_GPH3(5), S3C_GPIO_PULL_UP);
	s3c_gpio_cfgpin(S5PV210_GPH3(5), S3C_GPIO_SFN(0));

	s3c_gpio_setpull(S5PV210_GPH3(4), S3C_GPIO_PULL_UP);
	s3c_gpio_cfgpin(S5PV210_GPH3(4), S3C_GPIO_SFN(0));

	s3c_gpio_setpull(S5PV210_GPH0(7), S3C_GPIO_PULL_UP);
	s3c_gpio_cfgpin(S5PV210_GPH0(7), S3C_GPIO_SFN(0));

	s3c_gpio_setpull(S5PV210_GPH2(0), S3C_GPIO_PULL_UP);
	s3c_gpio_cfgpin(S5PV210_GPH2(0), S3C_GPIO_SFN(0));

	s3c_gpio_setpull(S5PV210_GPH1(1), S3C_GPIO_PULL_UP);
	s3c_gpio_cfgpin(S5PV210_GPH1(1), S3C_GPIO_SFN(0));

	s3c_button_history[0] = gpio_get_value(S5PV210_GPH0(1));
	s3c_button_history[1] = gpio_get_value(S5PV210_GPH0(4));
	s3c_button_history[2] = gpio_get_value(S5PV210_GPH3(5));
	s3c_button_history[3] = gpio_get_value(S5PV210_GPH3(4));
	s3c_button_history[4] = gpio_get_value(S5PV210_GPH0(7));
	s3c_button_history[5] = gpio_get_value(S5PV210_GPH2(0));
	s3c_button_history[6] = gpio_get_value(S5PV210_GPH1(1));

	input = input_allocate_device();
	if(!input) 
		return -ENOMEM;

	set_bit(EV_KEY, input->evbit);
	//set_bit(EV_REP, input->evbit);	/* Repeat Key */

	for(i = 0; i < MAX_BUTTON_CNT; i++)
		set_bit(s3c_Keycode[i], input->keybit);

	input->name = "s3c-button";
	input->phys = "s3c-button/input0";

	input->id.bustype = BUS_HOST;
	input->id.vendor = 0x0001;
	input->id.product = 0x0001;
	input->id.version = 0x0100;

	input->keycode = s3c_Keycode;

	if(input_register_device(input) != 0)
	{
		printk("s3c-button input register device fail!!\n");

		input_free_device(input);
		return -ENODEV;
	}

	/* Scan timer init */
	init_timer(&timer);
	timer.function = s3cbutton_timer_handler;

	timer.expires = jiffies + (HZ/100);
	add_timer(&timer);

	printk("s3c button Initialized!!\n");

	return 0;
}

static int s3c_button_remove(struct platform_device *pdev)
{
	input_unregister_device(input);
	del_timer(&timer);
	
	return  0;
}


#ifdef CONFIG_PM
static int s3c_button_suspend(struct platform_device *pdev, pm_message_t state)
{
	return 0;
}

static int s3c_button_resume(struct platform_device *pdev)
{
	return 0;
}
#else
#define s3c_button_suspend	NULL
#define s3c_button_resume	NULL
#endif

static struct platform_driver s3c_button_device_driver = {
	.probe		= s3c_button_probe,
	.remove		= s3c_button_remove,
	.suspend	= s3c_button_suspend,
	.resume		= s3c_button_resume,
	.driver		= {
		.name	= "s3c-button",
		.owner	= THIS_MODULE,
	}
};


static struct platform_device s3c_device_button = {
	.name	= "s3c-button",
	.id		= -1,
};

static int __init s3c_button_init(void)
{   
	platform_device_register(&s3c_device_button);       

	return platform_driver_register(&s3c_button_device_driver);
}

static void __exit s3c_button_exit(void)
{
	platform_driver_unregister(&s3c_button_device_driver);
	platform_device_unregister(&s3c_device_button);   
}

module_init(s3c_button_init);
module_exit(s3c_button_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("jerry jianjun <jerryjianjun@gmail.com>");
MODULE_DESCRIPTION("Keyboard driver for s3c button.");
MODULE_ALIAS("platform:s3c-button");
