/*
 * linux/drivers/power/s3c_fake_battery.c
 *
 * Battery measurement code for S3C platform.
 *
 * based on palmtx_battery.c
 *
 * Copyright (C) 2009 Samsung Electronics.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/power_supply.h>
#include <linux/delay.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/irq.h>
#include <linux/wakelock.h>
#include <asm/mach-types.h>
#include <mach/hardware.h>
#include <plat/gpio-cfg.h>

typedef enum {
	CHARGER_BATTERY = 0,
	CHARGER_USB,
	CHARGER_AC,
	CHARGER_DISCHARGE
} charger_type_t;

enum {
	BATT_VOL = 0,
	BATT_VOL_ADC,
	BATT_VOL_ADC_CAL,
	BATT_TEMP,
	BATT_TEMP_ADC,
	BATT_TEMP_ADC_CAL,
};

struct battery_info {
	u32 batt_id;			/* Battery ID from ADC */
	u32 batt_vol;			/* Battery voltage from ADC */
	u32 batt_vol_adc;		/* Battery ADC value */
	u32 batt_vol_adc_cal;	/* Battery ADC value (calibrated)*/
	u32 batt_temp;			/* Battery Temperature (C) from ADC */
	u32 batt_temp_adc;		/* Battery Temperature ADC value */
	u32 batt_temp_adc_cal;	/* Battery Temperature ADC value (calibrated) */
	u32 batt_current;		/* Battery current from ADC */
	u32 level;				/* formula */
	u32 charging_source;	/* 0: no cable, 1:usb, 2:AC */
	u32 charging_enabled;	/* 0: Disable, 1: Enable */
	u32 batt_health;		/* Battery Health (Authority) */
	u32 batt_is_full;       /* 0 : Not full 1: Full */
};

struct s3c_battery_info {
	int present;
	int polling;
	unsigned long polling_interval;

	struct battery_info bat_info;
};

#define SEC_BATTERY_ATTR(_name)														\
{																					\
	.attr = { .name = #_name, .mode = S_IRUGO | S_IWUGO, .owner = THIS_MODULE },	\
	.show = s3c_bat_show_property,													\
	.store = s3c_bat_store,															\
}

extern int s3c_adc_get_adc_data(int channel);
extern int s3c_adc_in_use;

static struct device * dev;
static struct s3c_battery_info s3c_bat_info;
static struct timer_list timer;
static int s3c_battery_initial = 0;

static void s3c_io_initial(void)
{
	gpio_request(S5PV210_GPH2(3), "GPH2");
	s3c_gpio_cfgpin(S5PV210_GPH2(3), S3C_GPIO_SFN(0));
	s3c_gpio_setpull(S5PV210_GPH2(3), S3C_GPIO_PULL_UP);

	gpio_request(S5PV210_GPH0(3), "GPH0");
	s3c_gpio_cfgpin(S5PV210_GPH0(3), S3C_GPIO_SFN(0));
	s3c_gpio_setpull(S5PV210_GPH0(3), S3C_GPIO_PULL_UP);
}

static int s3c_io_get_ac_status(void)
{
	return gpio_get_value(S5PV210_GPH2(3));
}

static int s3c_io_get_charge_status(void)
{
	return gpio_get_value(S5PV210_GPH0(3));
}

static ssize_t s3c_bat_show_property(struct device * dev, struct device_attribute * attr, char * buf);
static ssize_t s3c_bat_store(struct device * dev, struct device_attribute * attr, const char * buf, size_t count);

static struct device_attribute s3c_battery_attrs[] = {
	SEC_BATTERY_ATTR(batt_vol),
	SEC_BATTERY_ATTR(batt_vol_adc),
	SEC_BATTERY_ATTR(batt_vol_adc_cal),
	SEC_BATTERY_ATTR(batt_temp),
	SEC_BATTERY_ATTR(batt_temp_adc),
	SEC_BATTERY_ATTR(batt_temp_adc_cal),
};

static ssize_t s3c_bat_show_property(struct device * dev, struct device_attribute * attr, char * buf)
{
	int i = 0;
	const ptrdiff_t off = attr - s3c_battery_attrs;

	switch (off)
	{
	case BATT_VOL:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n", s3c_bat_info.bat_info.batt_vol);
	break;
	case BATT_VOL_ADC:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n", s3c_bat_info.bat_info.batt_vol_adc);
		break;
	case BATT_VOL_ADC_CAL:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n", s3c_bat_info.bat_info.batt_vol_adc_cal);
		break;
	case BATT_TEMP:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n", s3c_bat_info.bat_info.batt_temp);
		break;
	case BATT_TEMP_ADC:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n", s3c_bat_info.bat_info.batt_temp_adc);
		break;	
	case BATT_TEMP_ADC_CAL:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n", s3c_bat_info.bat_info.batt_temp_adc_cal);
		break;
	default:
		i = -EINVAL;
	}       

	return i;
}

static ssize_t s3c_bat_store(struct device * dev, struct device_attribute * attr, const char * buf, size_t count)
{
	int x = 0;
	int ret = 0;
	const ptrdiff_t off = attr - s3c_battery_attrs;

	switch (off)
	{
	case BATT_VOL_ADC_CAL:
		if (sscanf(buf, "%d\n", &x) == 1)
		{
			s3c_bat_info.bat_info.batt_vol_adc_cal = x;
			ret = count;
		}
		break;
	case BATT_TEMP_ADC_CAL:
		if (sscanf(buf, "%d\n", &x) == 1)
		{
			s3c_bat_info.bat_info.batt_temp_adc_cal = x;
			ret = count;
		}
		break;
	default:
		ret = -EINVAL;
	}       

	return ret;
}

static int s3c_bat_create_attrs(struct device * dev)
{
	int i, rc;

	for (i = 0; i < ARRAY_SIZE(s3c_battery_attrs); i++)
	{
		rc = device_create_file(dev, &s3c_battery_attrs[i]);
		if(rc)
			goto s3c_attrs_failed;
	}
	goto succeed;

s3c_attrs_failed:
	while (i--)
	device_remove_file(dev, &s3c_battery_attrs[i]);
succeed:
	return rc;
}

static enum power_supply_property s3c_battery_properties[] = {
	POWER_SUPPLY_PROP_STATUS,
	POWER_SUPPLY_PROP_HEALTH,
	POWER_SUPPLY_PROP_PRESENT,
	POWER_SUPPLY_PROP_TECHNOLOGY,
	POWER_SUPPLY_PROP_CAPACITY,
};

static enum power_supply_property s3c_power_properties[] = {
	POWER_SUPPLY_PROP_ONLINE,
};

static char *supply_list[] = {
	"battery",
};

static int s3c_bat_get_charging_status(void)
{
	charger_type_t charger = CHARGER_BATTERY; 
	int ret = 0;
        
	charger = s3c_bat_info.bat_info.charging_source;
        
	switch (charger)
	{
	case CHARGER_BATTERY:
		ret = POWER_SUPPLY_STATUS_NOT_CHARGING;
		break;
	case CHARGER_USB:
	case CHARGER_AC:
		if (s3c_bat_info.bat_info.level == 100 && s3c_bat_info.bat_info.batt_is_full)
		{
			ret = POWER_SUPPLY_STATUS_FULL;
		}
		else
		{
			ret = POWER_SUPPLY_STATUS_CHARGING;
		}
		break;
	case CHARGER_DISCHARGE:
		ret = POWER_SUPPLY_STATUS_DISCHARGING;
		break;
	default:
		ret = POWER_SUPPLY_STATUS_UNKNOWN;
	}

	return ret;
}

static int s3c_bat_get_property(struct power_supply *bat_ps, enum power_supply_property psp, union power_supply_propval *val)
{
	switch (psp)
	{
	case POWER_SUPPLY_PROP_STATUS:
		val->intval = s3c_bat_get_charging_status();
		break;
	case POWER_SUPPLY_PROP_HEALTH:
		val->intval = s3c_bat_info.bat_info.batt_health;
		break;
	case POWER_SUPPLY_PROP_PRESENT:
		val->intval = s3c_bat_info.present;
		break;
	case POWER_SUPPLY_PROP_TECHNOLOGY:
		val->intval = POWER_SUPPLY_TECHNOLOGY_LION;
		break;
	case POWER_SUPPLY_PROP_CAPACITY:
		val->intval = s3c_bat_info.bat_info.level;
		break;
	case POWER_SUPPLY_PROP_TEMP:
		val->intval = s3c_bat_info.bat_info.batt_temp;
		break;
	default:
		return -EINVAL;
	}
	return 0;
}

static int s3c_power_get_property(struct power_supply *bat_ps, enum power_supply_property psp, union power_supply_propval *val)
{
	charger_type_t charger = s3c_bat_info.bat_info.charging_source;

	switch (psp)
	{
	case POWER_SUPPLY_PROP_ONLINE:
		if (bat_ps->type == POWER_SUPPLY_TYPE_MAINS)
			val->intval = (charger == CHARGER_AC ? 1 : 0);
		else if (bat_ps->type == POWER_SUPPLY_TYPE_USB)
			val->intval = (charger == CHARGER_USB ? 1 : 0);
		else
			val->intval = 0;
		break;
	default:
		return -EINVAL;
	}
	
	return 0;
}

static struct power_supply s3c_power_supplies[] = {
	{
		.name = "battery",
		.type = POWER_SUPPLY_TYPE_BATTERY,
		.properties = s3c_battery_properties,
		.num_properties = ARRAY_SIZE(s3c_battery_properties),
		.get_property = s3c_bat_get_property,
	},
	{
		.name = "usb",
		.type = POWER_SUPPLY_TYPE_USB,
		.supplied_to = supply_list,
		.num_supplicants = ARRAY_SIZE(supply_list),
		.properties = s3c_power_properties,
		.num_properties = ARRAY_SIZE(s3c_power_properties),
		.get_property = s3c_power_get_property,
	},
	{
		.name = "ac",
		.type = POWER_SUPPLY_TYPE_MAINS,
		.supplied_to = supply_list,
		.num_supplicants = ARRAY_SIZE(supply_list),
		.properties = s3c_power_properties,
		.num_properties = ARRAY_SIZE(s3c_power_properties),
		.get_property = s3c_power_get_property,
	},
};

static int s3c_cable_status_update(int status)
{
	int ret = 0;
	charger_type_t source = CHARGER_BATTERY;

	if(!s3c_battery_initial)
		return -EPERM;

	switch(status)
	{
	case CHARGER_BATTERY:
		s3c_bat_info.bat_info.charging_source = CHARGER_BATTERY;
		break;
	case CHARGER_USB:
		s3c_bat_info.bat_info.charging_source = CHARGER_USB;
		break;
	case CHARGER_AC:
		s3c_bat_info.bat_info.charging_source = CHARGER_AC;
		break;
	case CHARGER_DISCHARGE:
		s3c_bat_info.bat_info.charging_source = CHARGER_DISCHARGE;
		break;
	default:
		ret = -EINVAL;
	}
	source = s3c_bat_info.bat_info.charging_source;

	/* if the power source changes, all power supplies may change state */
	power_supply_changed(&s3c_power_supplies[CHARGER_BATTERY]);
	// power_supply_changed(&s3c_power_supplies[CHARGER_USB]);
	// power_supply_changed(&s3c_power_supplies[CHARGER_AC]);

	return ret;
}

void s3c_cable_check_status(int flag)
{
    charger_type_t status = 0;

    if (flag == 0)			/* Battery */
		status = CHARGER_BATTERY;
    else if(flag ==1)		/* USB */
		status = CHARGER_USB;
    else if(flag ==2)		/* AC */
		status = CHARGER_AC;

    s3c_cable_status_update(status);
}
EXPORT_SYMBOL(s3c_cable_check_status);

static int s3c_adc_get_data_with_filter(int channel)
{
	int adc_data;
	int adc_max = 0;
	int adc_min = 0;
	int adc_total = 0;
	int i;

	for (i = 0; i < 6; i++)
	{
		do{
			if(s3c_adc_in_use != 0)
				return 0;
			adc_data = s3c_adc_get_adc_data(channel);
		}while(adc_data <=0);

		if (i != 0)
		{
			if (adc_data > adc_max)
				adc_max = adc_data;
			else if (adc_data < adc_min)
				adc_min = adc_data;
		}
		else
		{
			adc_max = adc_data;
			adc_min = adc_data;
		}
		adc_total += adc_data;
	}

	return (adc_total - adc_max - adc_min) / (6 - 2);
}

static int adc_buf[8];
static int adc_buf_index = 0;

static int vol_to_level(int vol)
{
	const int table_voltage[] = { 3400*2, 3572*2, 3608*2, 3645*2, 3688*2, 3719*2, 3768*2, 3817*2, 3866*2, 3976*2, 4086*2 };
	const int table_level[]   = { 0,      10,     20,     30,     40,     50,     60,     70,     80,     90,     100    };
	const int internal_resistance = 125;
	const int charge_current = 800;
	const int discharge_current = 350;
	int no_load_voltage;
	int level;
	int i;

	if(s3c_io_get_charge_status() != 0) /* charging */
		no_load_voltage = vol - (charge_current * internal_resistance / 1000);
	else
		no_load_voltage = vol + (discharge_current * internal_resistance / 1000);

	if(no_load_voltage < table_voltage[0])
		no_load_voltage = table_voltage[0];
	if(no_load_voltage > table_voltage[ARRAY_SIZE(table_voltage) - 1])
		no_load_voltage = table_voltage[ARRAY_SIZE(table_voltage) - 1];

	for(i = 1; i < ARRAY_SIZE(table_voltage); i++)
	{
		if(no_load_voltage < table_voltage[i])
		{
			level = table_level[i - 1] + (table_level[i] - table_level[i - 1]) * (no_load_voltage - table_voltage[i - 1]) / (table_voltage[i] - table_voltage[i - 1]);
			break;
		}
		else
		{
			level = table_level[i];
		}
	}

	if(level < 0)
		level = 0;
	if(level > 100)
		level = 100;

	return level;
}

static void s3c_get_bat_information(void)
{	
	int vol;
	int value;

	if(s3c_adc_in_use != 0)
		return;

	value = s3c_adc_get_data_with_filter(0);
	if(value <= 0)
		return;

	adc_buf[adc_buf_index] = value;
	adc_buf_index = (adc_buf_index + 1) & 0x7;

	vol = ((adc_buf[0] + adc_buf[1] + adc_buf[2] + adc_buf[3] + adc_buf[4] + adc_buf[5] + adc_buf[6] + adc_buf[7]) >> 3);
	vol = ( ((vol * 3300) / 4096) * 3 ) / 1;

	s3c_bat_info.bat_info.batt_temp = 200;
	s3c_bat_info.bat_info.batt_vol = vol;
	if(s3c_io_get_charge_status() != 0)
		s3c_bat_info.bat_info.level = 100;
	else
		s3c_bat_info.bat_info.level = vol_to_level(vol);

	/* For Debug */
	//printk("value = %d, vol = %d, level= %d\r\n", value, s3c_bat_info.bat_info.batt_vol, s3c_bat_info.bat_info.level);
}

static void s3c_bat_status_update(struct power_supply *bat_ps)
{
	int old_level, old_temp, old_batt_vol, old_is_full;
	int flag;
	static int history = 0;

	if(!s3c_battery_initial)
		return;

	old_temp = s3c_bat_info.bat_info.batt_temp;
	old_level = s3c_bat_info.bat_info.level;
	old_batt_vol = s3c_bat_info.bat_info.batt_vol;
	old_is_full = s3c_bat_info.bat_info.batt_is_full;

	flag = s3c_io_get_ac_status();
	if(flag != history)
	{
		if(flag)
			s3c_cable_check_status(2);
		else
			s3c_cable_check_status(0);
	}
	history = flag;	

	s3c_get_bat_information();

	if(old_level != s3c_bat_info.bat_info.level 
			|| old_temp != s3c_bat_info.bat_info.batt_temp
			|| old_is_full != s3c_bat_info.bat_info.batt_is_full
			|| old_batt_vol != s3c_bat_info.bat_info.batt_vol)
	{
		power_supply_changed(bat_ps);
	}
}

static void bat_adc_timer_handler(unsigned long data)
{
	s3c_bat_status_update(&s3c_power_supplies[CHARGER_BATTERY]);
	
	/* Kernel Timer restart */
	mod_timer(&timer,jiffies + (1 * HZ));
}

static int s3c_bat_probe(struct platform_device * pdev)
{
	int i;
	int ret;
	int value;

	dev = &pdev->dev;

	value = s3c_adc_get_adc_data(0);
	adc_buf[0] = value;
	adc_buf[1] = value;
	adc_buf[2] = value;
	adc_buf[3] = value;
	adc_buf[4] = value;
	adc_buf[5] = value;
	adc_buf[6] = value;
	adc_buf[7] = value;
	adc_buf_index = 0;

	s3c_bat_info.present = 1;
	s3c_bat_info.polling = 1;
	s3c_bat_info.bat_info.batt_id = 0;
	s3c_bat_info.bat_info.batt_vol = 0;
	s3c_bat_info.bat_info.batt_vol_adc = 0;
	s3c_bat_info.bat_info.batt_vol_adc_cal = 0;
	s3c_bat_info.bat_info.batt_temp = 0;
	s3c_bat_info.bat_info.batt_temp_adc = 0;
	s3c_bat_info.bat_info.batt_temp_adc_cal = 0;
	s3c_bat_info.bat_info.batt_current = 0;
	s3c_bat_info.bat_info.level = 0;
	s3c_bat_info.bat_info.charging_source = CHARGER_BATTERY;
	s3c_bat_info.bat_info.charging_enabled = 0;
	s3c_bat_info.bat_info.batt_health = POWER_SUPPLY_HEALTH_GOOD;

	s3c_io_initial();

	/* init power supplier framework */
	for (i = 0; i < ARRAY_SIZE(s3c_power_supplies); i++)
	{
		ret = power_supply_register(&pdev->dev, &s3c_power_supplies[i]);
		if(ret)
		{
			printk("Failed to register power supply %d\n", i);
			return ret;
		}
	}

	/* create sec detail attributes */
	s3c_bat_create_attrs(s3c_power_supplies[CHARGER_BATTERY].dev);
	
	s3c_battery_initial = 1;
	s3c_bat_status_update(&s3c_power_supplies[CHARGER_BATTERY]);

	/* Scan timer init */
	init_timer(&timer);
	timer.function = bat_adc_timer_handler;
	timer.expires = jiffies + (2 * HZ);
	add_timer(&timer);

	return 0;
}

static int s3c_bat_remove(struct platform_device * pdev)
{
	int i;

	if(s3c_bat_info.polling)
		del_timer_sync(&timer);

	for(i = 0; i < ARRAY_SIZE(s3c_power_supplies); i++)
	{
		power_supply_unregister(&s3c_power_supplies[i]);
	}
 
	return 0;
}

#ifdef CONFIG_PM
static int s3c_bat_suspend(struct platform_device * pdev, pm_message_t state)
{
	return 0;
}

static int s3c_bat_resume(struct platform_device * pdev)
{
	return 0;
}
#else
#define s3c_bat_suspend	NULL
#define s3c_bat_resume	NULL
#endif

static struct platform_driver s3c_bat_driver = {
	.driver = {
		.name		= "sec-fake-battery",
		.owner		= THIS_MODULE,
	},
	.probe			= s3c_bat_probe,
	.remove			= s3c_bat_remove,
	.suspend		= s3c_bat_suspend,
	.resume			= s3c_bat_resume,
};

static int __init s3c_bat_init(void)
{
	return platform_driver_register(&s3c_bat_driver);
}

static void __exit s3c_bat_exit(void)
{
	platform_driver_unregister(&s3c_bat_driver);
}

module_init(s3c_bat_init);
module_exit(s3c_bat_exit);

MODULE_AUTHOR("HuiSung Kang <hs1218.kang@samsung.com>");
MODULE_DESCRIPTION("S5PV210 battery driver for SMDKV210");
MODULE_LICENSE("GPL");
