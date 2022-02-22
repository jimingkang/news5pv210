/*
 *  Copyright (C) 2008-2009 Samsung Electronics
 *  Minkyu Kang <mk7.kang@samsung.com>
 *  Kyungmin Park <kyungmin.park@samsung.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/arch/gpio.h>
#include <asm/arch/mmc.h>
#include <power/pmic.h>
#include <usb/s3c_udc.h>
#include <asm/arch/cpu.h>
#include <power/max8998_pmic.h>
#include <s5pc110.h>
DECLARE_GLOBAL_DATA_PTR;

static struct s5pc110_gpio *s5pc110_gpio;


static void dm9000_pre_init(void)
{
	unsigned int tmp;

#if defined(DM9000_16BIT_DATA)
//	SROM_BW_REG &= ~(0xf << 20);
//	SROM_BW_REG |= (0<<23) | (0<<22) | (0<<21) | (1<<20);
	SROM_BW_REG &= ~(0xf << 4);
	SROM_BW_REG |= (1<<7) | (1<<6) | (1<<5) | (1<<4);

#else	
	SROM_BW_REG &= ~(0xf << 20);
	SROM_BW_REG |= (0<<19) | (0<<18) | (0<<16);
#endif
//	SROM_BC5_REG = ((0<<28)|(1<<24)|(5<<16)|(1<<12)|(4<<8)|(6<<4)|(0<<0));
	SROM_BC1_REG = ((0<<28)|(1<<24)|(5<<16)|(1<<12)|(4<<8)|(6<<4)|(0<<0));

	tmp = MP01CON_REG;
//	tmp &=~(0xf<<20);
//	tmp |=(2<<20);
	tmp &=~(0xf<<4);
	tmp |=(2<<4);

	MP01CON_REG = tmp;
}



int board_init(void)
{
	/* Set Initial global variables */
	s5pc110_gpio = (struct s5pc110_gpio *)S5PC110_GPIO_BASE;

	//gd->bd->bi_arch_number = MACH_TYPE_SMDKV210;		// ���������
	gd->bd->bi_arch_number = MACH_TYPE_SMDKV210;		// ���������
	gd->bd->bi_boot_params = PHYS_SDRAM_1 + 0x100;

// ���������ʼ��
#ifdef CONFIG_DRIVER_DM9000
	dm9000_pre_init();
#endif

	return 0;
}

int power_init_board(void)
{
#if 0
	int ret;

	/*
	 * For PMIC the I2C bus is named as I2C5, but it is connected
	 * to logical I2C adapter 0
	 */
	ret = pmic_init(I2C_0);
	if (ret)
		return ret;
#endif
	return 0;
}

int dram_init(void)
{
	gd->ram_size = PHYS_SDRAM_1_SIZE + PHYS_SDRAM_2_SIZE;

	return 0;
}

void dram_init_banksize(void)
{
	gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
	gd->bd->bi_dram[0].size = PHYS_SDRAM_1_SIZE;
	gd->bd->bi_dram[1].start = PHYS_SDRAM_2;
	gd->bd->bi_dram[1].size = PHYS_SDRAM_2_SIZE;
	
}

#ifdef CONFIG_DISPLAY_BOARDINFO
int checkboard(void)
{
	//puts("Board:\tGoni\n");
	puts("Board:\tAston210\n");
	return 0;
}
#endif

#ifdef CONFIG_GENERIC_MMC
int board_mmc_init(bd_t *bis)
{
#ifdef CONFIG_S3C_HSMMC
	setup_hsmmc_clock();
	setup_hsmmc_cfg_gpio();
	return smdk_s3c_hsmmc_init();
#else
	return 0;
#endif

	

#if 0
	int i, ret, ret_sd = 0;

	/* MASSMEMORY_EN: XMSMDATA7: GPJ2[7] output high */
	s5p_gpio_direction_output(&s5pc110_gpio->j2, 7, 1);			// GPJ2_7

	/*
	 * MMC0 GPIO
	 * GPG0[0]	SD_0_CLK
	 * GPG0[1]	SD_0_CMD
	 * GPG0[2]	SD_0_CDn	-> Not used
	 * GPG0[3:6]	SD_0_DATA[0:3]
	 */
	for (i = 0; i < 7; i++) {
		if (i == 2)
			continue;
		/* GPG0[0:6] special function 2 */
		s5p_gpio_cfg_pin(&s5pc110_gpio->g0, i, 0x2);		// GPG0_i 0x2
		/* GPG0[0:6] pull disable */
		s5p_gpio_set_pull(&s5pc110_gpio->g0, i, GPIO_PULL_NONE);
		/* GPG0[0:6] drv 4x */
		s5p_gpio_set_drv(&s5pc110_gpio->g0, i, GPIO_DRV_4X);
	}

	ret = s5p_mmc_init(0, 4);
	if (ret)
		error("MMC: Failed to init MMC:0.\n");

	for (i = 0; i < 7; i++) {
		if (i == 2)
			continue;

		/* GPG2[0:6] special function 2 */
		s5p_gpio_cfg_pin(&s5pc110_gpio->g2, i, 0x2);
		/* GPG2[0:6] pull disable */
		s5p_gpio_set_pull(&s5pc110_gpio->g2, i, GPIO_PULL_NONE);
		/* GPG2[0:6] drv 4x */
		s5p_gpio_set_drv(&s5pc110_gpio->g2, i, GPIO_DRV_4X);
	}

	ret_sd = s5p_mmc_init(2, 4);
	if (ret_sd)
		error("MMC: Failed to init SD card (MMC:2).\n");

#if 0
	/*
	 * SD card (T_FLASH) detect and init
	 * T_FLASH_DETECT: EINT28: GPH3[4] input mode
	 */
	s5p_gpio_cfg_pin(&s5pc110_gpio->h3, 4, GPIO_INPUT);
	s5p_gpio_set_pull(&s5pc110_gpio->h3, 4, GPIO_PULL_UP);

	if (!s5p_gpio_get_value(&s5pc110_gpio->h3, 4)) {
		for (i = 0; i < 7; i++) {
			if (i == 2)
				continue;

			/* GPG2[0:6] special function 2 */
			s5p_gpio_cfg_pin(&s5pc110_gpio->g2, i, 0x2);
			/* GPG2[0:6] pull disable */
			s5p_gpio_set_pull(&s5pc110_gpio->g2, i, GPIO_PULL_NONE);
			/* GPG2[0:6] drv 4x */
			s5p_gpio_set_drv(&s5pc110_gpio->g2, i, GPIO_DRV_4X);
		}

		ret_sd = s5p_mmc_init(2, 4);
		if (ret_sd)
			error("MMC: Failed to init SD card (MMC:2).\n");
	}
#endif

	return ret & ret_sd;
#endif
}
#endif

#ifdef CONFIG_USB_GADGET
static int s5pc1xx_phy_control(int on)
{
	int ret;
	static int status;
	struct pmic *p = pmic_get("MAX8998_PMIC");
	if (!p)
		return -ENODEV;

	if (pmic_probe(p))
		return -1;

	if (on && !status) {
		ret = pmic_set_output(p, MAX8998_REG_ONOFF1,
				      MAX8998_LDO3, LDO_ON);
		ret = pmic_set_output(p, MAX8998_REG_ONOFF2,
				      MAX8998_LDO8, LDO_ON);
		if (ret) {
			puts("MAX8998 LDO setting error!\n");
			return -1;
		}
		status = 1;
	} else if (!on && status) {
		ret = pmic_set_output(p, MAX8998_REG_ONOFF1,
				      MAX8998_LDO3, LDO_OFF);
		ret = pmic_set_output(p, MAX8998_REG_ONOFF2,
				      MAX8998_LDO8, LDO_OFF);
		if (ret) {
			puts("MAX8998 LDO setting error!\n");
			return -1;
		}
		status = 0;
	}
	udelay(10000);

	return 0;
}

struct s3c_plat_otg_data s5pc110_otg_data = {
	.phy_control = s5pc1xx_phy_control,
	.regs_phy = S5PC110_PHY_BASE,
	.regs_otg = S5PC110_OTG_BASE,
	.usb_phy_ctrl = S5PC110_USB_PHY_CONTROL,
};
#endif
