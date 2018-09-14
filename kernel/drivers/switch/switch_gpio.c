/*
 *  drivers/switch/switch_gpio.c
 *
 * Copyright (C) 2008 Google, Inc.
 * Author: Mike Lockwood <lockwood@android.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
*/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/switch.h>
#include <linux/workqueue.h>
#include <linux/gpio.h>

#include <plat/gpio-cfg.h>
#include <mach/regs-gpio.h>
#include <mach/regs-irq.h>
#include <linux/gpio.h>
#include <mach/irqs.h>
#include <linux/irq.h>
#include <linux/slab.h>


struct gpio_switch_data {
	struct switch_dev sdev;
	unsigned gpio;
	const char *name_on;
	const char *name_off;
	const char *state_on;
	const char *state_off;
	int irq;
	struct work_struct work;
};

static void gpio_switch_work(struct work_struct *work)
{
	int state;
	struct gpio_switch_data	*data =
		container_of(work, struct gpio_switch_data, work);

/*
	gpio_direction_input(S5PV210_GPH0(5));
	state = gpio_get_value(S5PV210_GPH0(5));
	s3c_gpio_cfgpin(S5PV210_GPH0(5), S3C_GPIO_SFN(0xf));

	if(state==0)
	{
		//printk("close speaker amp power\n");
		s3c_gpio_setpull(S5PV210_GPH3(2), S3C_GPIO_PULL_UP);
        s3c_gpio_cfgpin(S5PV210_GPH3(2), S3C_GPIO_SFN(1));
        gpio_set_value(S5PV210_GPH3(2), 0);
	}
	else if(state==1)
	{
		//printk("open speaker amp power\n");
		s3c_gpio_setpull(S5PV210_GPH3(2), S3C_GPIO_PULL_UP);
        s3c_gpio_cfgpin(S5PV210_GPH3(2), S3C_GPIO_SFN(1));
        gpio_set_value(S5PV210_GPH3(2), 1);
	}
*/	
	//switch_set_state(&data->sdev, !state);
}

static irqreturn_t gpio_irq_handler(int irq, void *dev_id)
{
	struct gpio_switch_data *switch_data =
	    (struct gpio_switch_data *)dev_id;
	
	schedule_work(&switch_data->work);
	return IRQ_HANDLED;
}

static ssize_t switch_gpio_print_state(struct switch_dev *sdev, char *buf)
{
	struct gpio_switch_data	*switch_data =
		container_of(sdev, struct gpio_switch_data, sdev);
	const char *state;
	if (switch_get_state(sdev))
		state = switch_data->state_on;
	else
		state = switch_data->state_off;

	if (state)
		return sprintf(buf, "%s\n", state);
	return -1;
}

static int gpio_switch_probe(struct platform_device *pdev)
{
	struct gpio_switch_platform_data *pdata = pdev->dev.platform_data;
	struct gpio_switch_data *switch_data;
	int ret = 0;

	if (!pdata)
		return -EBUSY;	

	switch_data = kzalloc(sizeof(struct gpio_switch_data), GFP_KERNEL);
	if (!switch_data)
		return -ENOMEM;
	
	switch_data->sdev.name = "h2w";
	switch_data->gpio = S5PV210_GPH1(0);
	switch_data->name_on = pdata->name_on;
	switch_data->name_off = pdata->name_off;
	switch_data->state_on = "1";
	switch_data->state_off = "0";
	switch_data->sdev.print_state = switch_gpio_print_state;

	switch_data->irq = IRQ_EINT5;
	
    ret = switch_dev_register(&switch_data->sdev);
	if (ret < 0)
		goto err_switch_dev_register;
	
/*
	ret = gpio_request(S5PV210_GPH0(5), "GPH0");
	if (ret < 0)
		goto err_request_gpio;	
	
	s3c_gpio_setpull(S5PV210_GPH0(5), S3C_GPIO_PULL_NONE);
    s3c_gpio_cfgpin(S5PV210_GPH0(5), S3C_GPIO_SFN(0xf));		
*/
	INIT_WORK(&switch_data->work, gpio_switch_work);

	ret = request_irq(switch_data->irq, gpio_irq_handler,
			  IRQ_TYPE_EDGE_BOTH, pdev->name, switch_data);
	if (ret < 0)
		goto err_request_irq;

	/* Perform initial detection */
	gpio_switch_work(&switch_data->work);

	return 0;

err_request_irq:
err_detect_irq_num_failed:
err_set_gpio_input:
	gpio_free(switch_data->gpio);
err_request_gpio:
    switch_dev_unregister(&switch_data->sdev);
err_switch_dev_register:
	kfree(switch_data);

	return ret;
}

static int __devexit gpio_switch_remove(struct platform_device *pdev)
{
	struct gpio_switch_data *switch_data = platform_get_drvdata(pdev);

	cancel_work_sync(&switch_data->work);
	gpio_free(switch_data->gpio);
    switch_dev_unregister(&switch_data->sdev);
	kfree(switch_data);

	return 0;
}

#ifdef CONFIG_PM
static int gpio_switch_suspend(struct platform_device *pdev, pm_message_t state)
{
/*
	printk("gpio_switch_suspend:close speaker amp power\n");
	s3c_gpio_setpull(S5PV210_GPH3(2), S3C_GPIO_PULL_UP);
    s3c_gpio_cfgpin(S5PV210_GPH3(2), S3C_GPIO_SFN(1));
    gpio_set_value(S5PV210_GPH3(2), 0);
*/
	return 0;
}

static int gpio_switch_resume(struct platform_device *pdev)
{
/*
	int state;

	gpio_direction_input(S5PV210_GPH0(5));
	state = gpio_get_value(S5PV210_GPH0(5));
	s3c_gpio_cfgpin(S5PV210_GPH0(5), S3C_GPIO_SFN(0xf));

	if(state==1)
	{
		//printk("gpio_switch_resume:open speaker amp power\n");
		s3c_gpio_setpull(S5PV210_GPH3(2), S3C_GPIO_PULL_UP);
        s3c_gpio_cfgpin(S5PV210_GPH3(2), S3C_GPIO_SFN(1));
        gpio_set_value(S5PV210_GPH3(2), 1);
	}
*/	
	return 0;
}
#else
#define gpio_switch_suspend	NULL
#define gpio_switch_resume	NULL
#endif


static struct platform_driver gpio_switch_driver = {
	.probe		= gpio_switch_probe,
	.remove		= __devexit_p(gpio_switch_remove),
	.suspend	= gpio_switch_suspend,
	.resume		= gpio_switch_resume,
	.driver		= {
		.name	= "switch-gpio",
		.owner	= THIS_MODULE,
	},
};

static int __init gpio_switch_init(void)
{	
	return platform_driver_register(&gpio_switch_driver);
}

static void __exit gpio_switch_exit(void)
{	
	platform_driver_unregister(&gpio_switch_driver);
}

module_init(gpio_switch_init);
module_exit(gpio_switch_exit);

MODULE_AUTHOR("Mike Lockwood <lockwood@android.com>");
MODULE_DESCRIPTION("GPIO Switch driver");
MODULE_LICENSE("GPL");
