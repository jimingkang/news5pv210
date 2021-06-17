#include <linux/input.h> 
#include <linux/module.h> 
#include <linux/init.h>
#include <asm/irq.h> 
#include <asm/io.h>

#include <mach/irqs.h>			// arch/arm/mach-s5pv210/include/mach/irqs.h
#include <linux/interrupt.h>
#include <linux/gpio.h>

/*
 * X210:
 *
 * POWER  -> EINT1   -> GPH0_1
 * LEFT   -> EINT2   -> GPH0_2
 * DOWN   -> EINT3   -> GPH0_3
 * UP     -> KP_COL0 -> GPH2_0
 * RIGHT  -> KP_COL1 -> GPH2_1
 * MENU   -> KP_COL3 -> GPH2_3 (KEY_A)
 * BACK   -> KP_COL2 -> GPH2_2 (KEY_B)
 */

#define BUTTON_IRQ	IRQ_EINT2

static struct input_dev *button_dev;

// 下半部函数
void func(unsigned long data)
{
	int flag;
	
	printk("key-s5pv210: this is bottom half\n");
	
	s3c_gpio_cfgpin(S5PV210_GPH0(2), S3C_GPIO_SFN(0x0));		// input模式
	flag = gpio_get_value(S5PV210_GPH0(2));
	s3c_gpio_cfgpin(S5PV210_GPH0(2), S3C_GPIO_SFN(0x0f));		// eint2模式

	input_report_key(button_dev, KEY_LEFT, !flag);
	input_sync(button_dev);
}

DECLARE_TASKLET(mytasklet, func, 0);

static irqreturn_t button_interrupt(int irq, void *dummy) 
{ 
	printk("key-s5pv210: this is top half\n");
	
	tasklet_schedule(&mytasklet);
	
	return IRQ_HANDLED; 
}

static int __init button_init(void) 
{ 
	int error;
	
	error = gpio_request(S5PV210_GPH0(2), "GPH0_2");
	if(error)
		printk("key-s5pv210: request gpio GPH0(2) fail");
	s3c_gpio_cfgpin(S5PV210_GPH0(2), S3C_GPIO_SFN(0x0f));		// eint2模式
	
	if (request_irq(BUTTON_IRQ, button_interrupt, IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, "button-x210", NULL)) 
	{ 
		printk(KERN_ERR "key-s5pv210.c: Can't allocate irq %d\n", BUTTON_IRQ);
		return -EBUSY; 
	}
	button_dev = input_allocate_device();
	if (!button_dev) 
	{ 
		printk(KERN_ERR "key-s5pv210.c: Not enough memory\n");
		error = -ENOMEM;
		goto err_free_irq; 
	}
	
	button_dev->evbit[0] = BIT_MASK(EV_KEY);
	button_dev->keybit[BIT_WORD(KEY_LEFT)] = BIT_MASK(KEY_LEFT);
	
	error = input_register_device(button_dev);
	if (error) 
	{ 
		printk(KERN_ERR "key-s5pv210.c: Failed to register device\n");
		goto err_free_dev; 
	}
	return 0;
	
err_free_dev:
	input_free_device(button_dev);
err_free_irq:
	free_irq(BUTTON_IRQ, button_interrupt);
	return error; 
}

static void __exit button_exit(void) 
{ 
	input_unregister_device(button_dev); 
	free_irq(BUTTON_IRQ, button_interrupt); 
}

module_init(button_init); 
module_exit(button_exit); 

MODULE_LICENSE("GPL");
MODULE_AUTHOR("aston <1264671872@qq.com>");
MODULE_DESCRIPTION("key driver for x210 button.");




