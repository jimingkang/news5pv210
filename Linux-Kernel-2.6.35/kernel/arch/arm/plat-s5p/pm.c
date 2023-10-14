/* linux/arch/arm/plat-s5p/pm.c
 *
 * Copyright (c) 2010 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 *
 * S5P Power Manager (Suspend-To-RAM) support
 *
 * Based on arch/arm/plat-s3c24xx/pm.c
 * Copyright (c) 2004,2006 Simtec Electronics
 *	Ben Dooks <ben@simtec.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include <linux/suspend.h>
#include <linux/gpio.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <plat/pm.h>

#define PFX "s5p pm: "

/* s3c_pm_configure_extint
 *
 * configure all external interrupt pins
*/

void s3c_pm_configure_extint(void)
{
	s3c_gpio_setpull(S5PV210_GPH0(1), S3C_GPIO_PULL_UP);
	s3c_gpio_cfgpin(S5PV210_GPH0(1), S3C_GPIO_SFN(0xf));

	enable_irq_wake(IRQ_EINT(1));
	//enable_irq(IRQ_EINT(1));
}

void s3c_pm_restore_core(void)
{
	/* nothing here yet */
}

void s3c_pm_save_core(void)
{
	/* nothing here yet */
}

