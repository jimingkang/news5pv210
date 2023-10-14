/* 
 * linux/drivers/char/sensor_KXTF9.c
 *
 * Author:  <linux@telechips.com>
 * Created: 10th Jun, 2008 
 * Description: Telechips Linux BACK-LIGHT DRIVER
 *
 * Copyright (c) Telechips, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <asm/uaccess.h>

#include <linux/slab.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/mach-types.h>
#include <linux/delay.h>
#include <linux/kxtf9.h>
#include <linux/miscdevice.h>
#include <linux/earlysuspend.h>

#ifdef CONFIG_I2C
#include <linux/i2c.h>
#endif
#include <asm/gpio.h>
#include <linux/input.h>

#ifdef CONFIG_HAS_WAKELOCK
static struct early_suspend	early_suspend;
#endif

#define KXTF9_DEBUG    0

#if KXTF9_DEBUG
#define sensor_dbg(fmt, arg...)     printk(fmt, ##arg)
#else
#define sensor_dbg(arg...)
#endif

#define SENSOR_DEV_NAME			"sensor"
#define SENSOR_DEV_MAJOR		252
#define SENSOR_DEV_MINOR		1
#define SENSOR_ID 1
#define SENSOR_12BITS

#ifdef SENSOR_12BITS
#define ST_RESP             0x0C
#define WHO_AM_I            0x0F
#define TILT_POS_CUR        0x10
#define TILT_POS_PRE        0x11
#define INT_SRC_REG1        0x15
#define INT_SRC_REG2        0x16
#define STATUS_REG          0x18
#define CTRL_REG1           0x1b
#define CTRL_REG2           0x1c
#define CTRL_REG3           0x1d
#define INT_CTRL_REG1       0x1e
#define INT_CTRL_REG2       0x1f
#define INT_CTRL_REG3       0x20
#define Xreg_L              0x06
#define Xreg_H              0x07
#define Yreg_L              0x08
#define Yreg_H              0x09
#define Zreg_L              0x0a  
#define Zreg_H              0x0b  
#else
#define ST_RESP             0x0c
#define WHO_AM_I            0x0F
#define TILT_POS_CUR        0x10
#define TILT_POS_PRE        0x11
#define INT_SRC_REG1        0x16
#define INT_SRC_REG2        0x17
#define STATUS_REG          0x18
#define CTRL_REG1           0x1b
#define CTRL_REG3           0x1d
#define INT_CTRL_REG1       0x1e
#define Xreg                0x12
#define Yreg                0x13
#define Zreg                0x14  
#endif

typedef struct {
    int x;
    int y;
    int z;
    int delay_time;
} s3c_sensor_accel_t;


#ifdef CONFIG_I2C
#define I2C_BUS					 (0)
#define KXTF9_I2C_ADDRESS      0x0F

static struct i2c_driver sensor_i2c_driver;
static struct i2c_client *sensor_i2c_client;

static struct timer_list *sensor_timer;
static int sensor_used_count=0;
volatile static unsigned int sensor_state_flag=0;
volatile static unsigned int sensor_duration=200;
volatile static s3c_sensor_accel_t s3c_sensor_accel;
static struct work_struct sensor_work_q;

static const struct i2c_device_id sensor_i2c_id[] = {
    { "s3c-Gsensor-kxtf9", 0, },
    { }
};

struct kxff9_data {
        struct input_dev *input_dev;
        struct work_struct work;
        struct early_suspend akm_early_suspend;
};

/***** kxtf9 functions ********************************************/
static int kxtf9_open(struct inode *inode, struct file *file)
{
    return 0;
}

static int kxtf9_release(struct inode *inode, struct file *file)
{
	return 0;
}

static int
kxtf9_ioctl(struct inode *inode, struct file *file, unsigned int cmd,
		   unsigned long arg)
{
	return 0;
}

/*********************************************/
/***** kxtf9_aot functions ***************************************/
static int kxtf9_aot_open(struct inode *inode, struct file *file)
{
	int ret = -1;

	return 0;
}

static int kxtf9_aot_release(struct inode *inode, struct file *file)
{
	return 0;
}

static int
kxtf9_aot_ioctl(struct inode *inode, struct file *file,
			  unsigned int cmd, unsigned long arg)
{
	void __user *argp = (void __user *)arg;
	short flag;
	
	switch (cmd) {
		case ECS_IOCTL_APP_SET_MFLAG:
		case ECS_IOCTL_APP_SET_AFLAG:
		case ECS_IOCTL_APP_SET_MVFLAG:
			if (copy_from_user(&flag, argp, sizeof(flag))) {
				return -EFAULT;
			}
			if (flag < 0 || flag > 1) {
				return -EINVAL;
			}
			break;
		case ECS_IOCTL_APP_SET_DELAY:
			if (copy_from_user(&flag, argp, sizeof(flag))) {
				return -EFAULT;
			}
			sensor_duration= (unsigned int)flag;
			sensor_dbg(KERN_INFO "%s:A IOCTL_SENSOR_SET_DELAY_ACCEL (0x%x) %d \n", __FUNCTION__, cmd, sensor_duration);
			if(sensor_duration>200) 
			    sensor_duration=200;
			else if(sensor_duration<30)
			    sensor_duration=30;
			sensor_dbg(KERN_INFO "%s:F IOCTL_SENSOR_SET_DELAY_ACCEL (0x%x) %d \n", __FUNCTION__, cmd, sensor_duration);
			s3c_sensor_accel.delay_time = sensor_duration;
			break;
		default:
			break;
	}
    return 0;
}

static struct file_operations kxtf9_fops = {
        .owner = THIS_MODULE,
        .open = kxtf9_open,
        .release = kxtf9_release,
        .ioctl = kxtf9_ioctl,
};

static struct file_operations kxtf9_aot_fops = {
        .owner = THIS_MODULE,
        .open = kxtf9_aot_open,
        .release = kxtf9_aot_release,
        .ioctl = kxtf9_aot_ioctl,
};

static struct miscdevice kxtf9_device = {
        .minor = MISC_DYNAMIC_MINOR,
        .name = "kxtf9_dev",
        .fops = &kxtf9_fops,
};

static struct miscdevice kxtf9_aot_device = {
        .minor = MISC_DYNAMIC_MINOR,
        .name = "kxtf9_aot",
        .fops = &kxtf9_aot_fops,
};
/*********************************************/

struct sensor_i2c_chip_info {
    unsigned gpio_start;
    uint16_t reg_output;
    uint16_t reg_direction;

    struct i2c_client *client;
    struct gpio_chip gpio_chip;
};

static void sensor_timer_handler(unsigned long data)
{
    sensor_dbg("%s\n", __func__);
    if (schedule_work(&sensor_work_q) == 0) {
        sensor_dbg("cannot schedule work !!!\n");
    }
}

static void sensor_timer_registertimer(struct timer_list* ptimer, unsigned int timeover )
{
    sensor_dbg("###%s\n", __func__);
    init_timer(ptimer);
    ptimer->expires = jiffies+msecs_to_jiffies(timeover);
    ptimer->data = (unsigned long)NULL;
    ptimer->function = sensor_timer_handler;

    add_timer(ptimer);
}

static void SENSOR_SEND_CMD(unsigned char reg, unsigned char val)
{
        unsigned char cmd[2];
        cmd[0] = reg;
        cmd[1] = val;
        i2c_master_send(sensor_i2c_client, cmd, 2);
}


static unsigned char SENSOR_READ_DAT(unsigned char reg)
{
        unsigned char buf;

        i2c_master_send(sensor_i2c_client, &reg, 1);
        i2c_master_recv(sensor_i2c_client, &buf, 1);

        return  buf;
}

static int sensor_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct sensor_i2c_chip_info 	*chip;

	sensor_dbg("\n sensor_i2c_probe  :  %s \n", client->name);

	chip = kzalloc(sizeof(struct sensor_i2c_chip_info), GFP_KERNEL);
	if(chip == NULL)
	{
		sensor_dbg("\n tcc_sensor_i2c  :  no chip info. \n");
		return -ENOMEM;
	}

	chip->client = client;
	i2c_set_clientdata(client, chip);
	//sensor_i2c_client = client;

	return 0;
}

static int sensor_i2c_remove(struct i2c_client *client)
{
	struct sensor_i2c_chip_info 		*chip  = i2c_get_clientdata(client);

	kfree(chip);
	//sensor_i2c_client = NULL;
	
	return 0;
}

static int KXTF9_INIT(void);
#ifdef CONFIG_PM
static int sensor_i2c_suspend(struct early_suspend *h)
{
	/* disable gsensor power */
	/*s3c_gpio_setpull(S5PV210_GPH3(1), S3C_GPIO_PULL_UP);
	s3c_gpio_cfgpin(S5PV210_GPH3(1), S3C_GPIO_SFN(1));
	gpio_set_value(S5PV210_GPH3(1), 0);*/

	return 0;
}

static int sensor_i2c_resume(struct early_suspend *h)
{
	/* enable gsensor power.x210ii: GPD0_3 */
	s3c_gpio_setpull(S5PV210_GPD0(3), S3C_GPIO_PULL_UP);
	s3c_gpio_cfgpin(S5PV210_GPD0(3), S3C_GPIO_SFN(1));
	gpio_set_value(S5PV210_GPD0(3), 1);

	printk("%s\n", __func__);
	KXTF9_INIT();

	return 0;
}
#else
#define sensor_i2c_suspend NULL
#define sensor_i2c_resume NULL
#endif

/* KXTF9 i2c control layer */
static struct i2c_driver sensor_i2c_driver = {
	.driver = {
		.name	= "s3c-Gsensor-kxtf9",
       	.owner  = THIS_MODULE,
	},
	.probe		= sensor_i2c_probe,
	.remove		= sensor_i2c_remove,
	//.suspend	= sensor_i2c_suspend,
	//.resume		= sensor_i2c_resume,
	.id_table	= sensor_i2c_id,
};

static int KXTF9_i2c_register(void)
{
    struct i2c_board_info info;
    struct i2c_adapter *adapter;
    struct i2c_client *client;
    
    memset(&info, 0, sizeof(struct i2c_board_info));
    info.addr = KXTF9_I2C_ADDRESS;
    strlcpy(info.type, "s3c-Gsensor-kxtf9", I2C_NAME_SIZE);

    sensor_dbg(KERN_INFO "%s : KXTF9_i2c_register\n", __FUNCTION__);
    	  

    adapter = i2c_get_adapter(I2C_BUS);
    if (!adapter) 
    {
        sensor_dbg(KERN_ERR "can't get i2c adapter 0 for s3c-Gsensor-kxtf9\n");
        return -ENODEV;
    }

    client = i2c_new_device(adapter, &info);
    i2c_put_adapter(adapter);
    if (!client) 
    {
        sensor_dbg(KERN_ERR "can't add i2c device at 0x%x\n", (unsigned int)info.addr);
        return -ENODEV;
    }

    sensor_i2c_client = client;

    return 0;
}

#endif  /* #ifdef CONFIG_I2C */

static int s3c_sensor_get_accel(void)
{
	unsigned  char  x_l, y_l, z_l,x_h, y_h, z_h;
	int x_sign,y_sign,z_sign;
	int x = 0;
	int y = 0;
	int z = 0;
	unsigned char tmp;
//	int data[3];
#ifdef SENSOR_12BITS
	x_l = SENSOR_READ_DAT(Xreg_L);
	x_h = SENSOR_READ_DAT(Xreg_H);
	y_l = SENSOR_READ_DAT(Yreg_L);
	y_h = SENSOR_READ_DAT(Yreg_H);
	z_l = SENSOR_READ_DAT(Zreg_L);
	z_h = SENSOR_READ_DAT(Zreg_H);

	x_l=x_l>>4;
	x=x_h*16+x_l;
	x_sign=x>>11;
	if(x_sign==1)
	{
		x = ((~(x) + 0x01) & 0x0FFF);	//2's complement
		x = -(x);
	}

	y_l=y_l>>4;
	y=y_h*16+y_l;
	y_sign=y>>11;
	if(y_sign==1)
	{
		y = ((~(y) + 0x01) & 0x0FFF);	//2's complement
		y = -(y);
	}

	z_l=z_l>>4;
	z=z_h*16+z_l;
	z_sign=z>>11;
	if(z_sign==1)
	{
		z = ((~(z) + 0x01) & 0x0FFF);	//2's complement
		z = -(z);
	}

	s3c_sensor_accel.x = y;
	s3c_sensor_accel.y = x;
	s3c_sensor_accel.z = (-1)*z;
#else
	x = (int)SENSOR_READ_DAT(Xreg);
	y = (int)SENSOR_READ_DAT(Yreg);
	z = (int)SENSOR_READ_DAT(Zreg);
	x=x>>2;
	s3c_sensor_accel.x=(x-32);
	y=y>>2;
	s3c_sensor_accel.y=(y-32);
	z=z>>2;
	s3c_sensor_accel.z=(z-32);
#endif

	//printk("%s: %d, %d, %d delay_time = %d\n", __func__, s3c_sensor_accel.x, s3c_sensor_accel.y, s3c_sensor_accel.z, s3c_sensor_accel.delay_time);
    return 0;
}

static void sensor_fetch_thread(struct work_struct *work)
{
    sensor_dbg("%s: sensor_duration =%d \n", __func__, sensor_duration);
    s3c_sensor_get_accel();
    sensor_timer_registertimer( sensor_timer, sensor_duration );
}

static ssize_t s3c_sensor_write(struct file *file, const char __user *user, size_t size, loff_t *o)
 {
	sensor_dbg("%s\n", __func__);
	return 0;
 }

static ssize_t s3c_sensor_read(struct file *file, char __user *user, size_t size, loff_t *o)
{ 
    sensor_dbg("%s\n", __func__);    
    sensor_dbg("%s: IOCTL_read_SENSORS_ACCELERATION  %d, %d, %d\n", __func__, s3c_sensor_accel.x, s3c_sensor_accel.y, s3c_sensor_accel.z);

    if(copy_to_user(( s3c_sensor_accel_t*) user, (const void *)&s3c_sensor_accel, sizeof( s3c_sensor_accel_t))!=0)
    {
        sensor_dbg("tcc_gsensor_read error\n");
    }
    return 0;
}

static int s3c_sensor_ioctl(struct inode *inode, struct file *filp, 
							unsigned int cmd, void *arg)
{
    printk("%s  (0x%x)  \n", __FUNCTION__, cmd);

	#if 0
    switch (cmd) {

        case IOCTL_SENSOR_GET_DATA_ACCEL:

            if(copy_to_user((s3c_sensor_accel_t*)arg, (const void *)&s3c_sensor_accel, sizeof(s3c_sensor_accel_t))!=0)
            {
                sensor_dbg("copy_to error\n");
            }
            sensor_dbg("%s: IOCTL_SENSOR_GET_DATA_ACCEL %d, %d, %d\n", __func__, s3c_sensor_accel.x, s3c_sensor_accel.y, s3c_sensor_accel.z);
            break;

        case IOCTL_SENSOR_SET_DELAY_ACCEL:
            if(copy_from_user((void *)&sensor_duration, (unsigned int*) arg, sizeof(unsigned int))!=0)
            {
                sensor_dbg("copy_from error\n");
            }					
            sensor_dbg(KERN_INFO "%s:  IOCTL_SENSOR_SET_DELAY_ACCEL (0x%x) %d \n", __FUNCTION__, cmd, sensor_duration);
            s3c_sensor_accel.delay_time = sensor_duration;
            break;

        case IOCTL_SENSOR_GET_STATE_ACCEL:
            sensor_state_flag = 1;
            if(copy_to_user((unsigned int*) arg, (const void *)&sensor_state_flag, sizeof(unsigned int))!=0)
            {
                sensor_dbg("copy_to error\n");
            }			
			
            sensor_dbg(KERN_INFO "%s: IOCTL_SENSOR_GET_STATE_ACCEL  (0x%x) %d \n", __FUNCTION__, cmd, sensor_state_flag);	
            break;
			
        case IOCTL_SENSOR_SET_STATE_ACCEL:
        //    arg = sensor_state_flag;
	
            if(copy_from_user((void *)&sensor_state_flag, (unsigned int*) arg, sizeof(unsigned int))!=0)
            {
                sensor_dbg("copy_from error\n");
            }			
            sensor_dbg(KERN_INFO "%s: IOCTL_SENSOR_SET_STATE_ACCEL  (0x%x) %d \n", __FUNCTION__, cmd, sensor_state_flag);			
            break;
			
        default:
            sensor_dbg("sensor: unrecognized ioctl (0x%x)\n", cmd); 
            return -EINVAL;
            break;
    }
	#endif
    return 0;
}

static int s3c_sensor_release(struct inode *inode, struct file *filp)
{
    sensor_dbg("%s (%d)\n", __FUNCTION__, sensor_used_count);
    sensor_used_count--;
    if (sensor_used_count < 0) {
        sensor_dbg("sensor: release error (over)\n"); 
        sensor_used_count = 0;
    }

    if (sensor_used_count == 0) {
        flush_scheduled_work();      
        del_timer_sync( sensor_timer );
        //del_timer( sensor_timer);
        kfree( sensor_timer);

#ifdef CONFIG_I2C
        i2c_unregister_device(sensor_i2c_client);
        i2c_del_driver(&sensor_i2c_driver);
        sensor_i2c_client = NULL;
#endif
    }
    return 0;
}

static int kxtf9_sleep(void)
{
        unsigned char tmp=0,i=0;
        tmp = SENSOR_READ_DAT(CTRL_REG1);
        tmp = tmp&0x7f;
        SENSOR_SEND_CMD(CTRL_REG1,tmp);
	return 0;
}

static int KXTF9_RESET(void)
{
	unsigned char tmp=0,i=0;
	tmp = SENSOR_READ_DAT(CTRL_REG3);
	tmp = tmp|0x80;
	SENSOR_SEND_CMD(CTRL_REG3,tmp);
	msleep(50);
	tmp = SENSOR_READ_DAT(CTRL_REG3);
	return 0;
}

static int KXTF9_INIT(void)
{
	unsigned char dat;

#ifdef SENSOR_12BITS
	kxtf9_sleep();
	KXTF9_RESET();

	dat = SENSOR_READ_DAT(WHO_AM_I);
	SENSOR_SEND_CMD(CTRL_REG1,0x00);
	SENSOR_SEND_CMD(INT_CTRL_REG1,0x1c);
	SENSOR_SEND_CMD(CTRL_REG1,0x61);
	SENSOR_SEND_CMD(CTRL_REG1,0xE1);
#else
	SENSOR_SEND_CMD(CTRL_REG1,0x00);
	dat = SENSOR_READ_DAT(WHO_AM_I);
	SENSOR_SEND_CMD(CTRL_REG1,0x19);
	SENSOR_SEND_CMD(INT_CTRL_REG1,0x1c);
	SENSOR_SEND_CMD(CTRL_REG1,0x99);
#endif

	return 0;
}

static int s3c_sensor_open(struct inode *inode, struct file *filp)
{
    int ret;
    unsigned char old_ctrl;
    static int device_init=0;	
    sensor_dbg("** %s **\n",__FUNCTION__);
    //int num = MINOR(inode->i_rdev);
    if (sensor_used_count == 0) {
#ifdef CONFIG_I2C
        // Initialize I2C driver for KXTF9
        ret = i2c_add_driver(&sensor_i2c_driver);
        if(ret < 0) 
        {
            sensor_dbg("%s() [Error] failed i2c_add_driver() = %d\n", __func__, ret);
            return ret;
        }
        ret = KXTF9_i2c_register();
        if(ret < 0) 
        {
            sensor_dbg("%s() [Error] Failed register i2c client driver for KXTF9, return is %d\n", __func__, ret);
            return ret;
        }
        sensor_dbg("%s: post KXTF9_i2c_register : %x\n", __func__, old_ctrl);
#endif
	if(device_init==0)
	{
		#ifdef SENSOR_12BITS
			kxtf9_sleep();
			KXTF9_RESET();
			printk("*** 12 BitS ***\n");
			old_ctrl = SENSOR_READ_DAT(WHO_AM_I);
			SENSOR_SEND_CMD(CTRL_REG1,0x00);
			SENSOR_SEND_CMD(INT_CTRL_REG1,0x1c);
			SENSOR_SEND_CMD(CTRL_REG1,0x61);
			SENSOR_SEND_CMD(CTRL_REG1,0xE1);
			printk("%s: ASIC revition ID : %x\n", __func__, old_ctrl);
		#else
			printk("** 6 BitS **\n");
			SENSOR_SEND_CMD(CTRL_REG1,0x00);
			old_ctrl = SENSOR_READ_DAT(WHO_AM_I);
			SENSOR_SEND_CMD(CTRL_REG1,0x19);
			SENSOR_SEND_CMD(INT_CTRL_REG1,0x1c);
			SENSOR_SEND_CMD(CTRL_REG1,0x99);
			printk("%s: ASIC revition ID : %x\n", __func__, old_ctrl);
		#endif
		device_init=1;
	}

        sensor_timer= kmalloc( sizeof( struct timer_list ), GFP_KERNEL );      // test
        if (sensor_timer == NULL){
            sensor_dbg("%s: mem alloc fail\n", __func__);
#ifdef CONFIG_I2C
            i2c_unregister_device(sensor_i2c_client);
            i2c_del_driver(&sensor_i2c_driver);
            sensor_i2c_client = NULL;
#endif
            return -ENOMEM;
        }
        memset(sensor_timer, 0, sizeof(struct timer_list));      // test
        sensor_timer_registertimer( sensor_timer, sensor_duration );      // test
    }
    sensor_used_count++;	
    sensor_dbg("%s out... \n", __FUNCTION__);
    return 0;
}

struct file_operations s3c_sensor_fops =
{
	.owner		= THIS_MODULE,
	.open		= s3c_sensor_open,
	.release		= s3c_sensor_release,
	.ioctl			= s3c_sensor_ioctl,
	.read      		= s3c_sensor_read,
	.write		= s3c_sensor_write,	
};

static struct class *sensor_class;

static void init_event(void)
{
        struct kxff9_data *akm;
        int err = 0;
        /* Allocate memory for driver data */
        akm = kzalloc(sizeof(struct kxff9_data), GFP_KERNEL);
        if (!akm) {
                printk(KERN_ERR "kxtf9 init_event: memory allocation failed.\n");
                err = -ENOMEM;
                goto exit1;
        }
        /* Declare input device */
        akm->input_dev = input_allocate_device();
        if (!akm->input_dev) {
                err = -ENOMEM;
                printk(KERN_ERR
                       "kxtf9 init_event: Failed to allocate input device\n");
                goto exit5;
        }
        /* Setup input device */
        set_bit(EV_ABS, akm->input_dev->evbit);
        /* yaw (0, 360) */
        input_set_abs_params(akm->input_dev, ABS_RX, 0, 23040, 0, 0);
        /* pitch (-180, 180) */
        input_set_abs_params(akm->input_dev, ABS_RY, -11520, 11520, 0, 0);
        /* roll (-90, 90) */
        input_set_abs_params(akm->input_dev, ABS_RZ, -5760, 5760, 0, 0);
        /* x-axis acceleration */
        input_set_abs_params(akm->input_dev, ABS_X, -5760, 5760, 0, 0);
        /* y-axis acceleration (720 x 8G) */
        input_set_abs_params(akm->input_dev, ABS_Y, -5760, 5760, 0, 0);
        /* z-axis acceleration (720 x 8G) */
        input_set_abs_params(akm->input_dev, ABS_Z, -5760, 5760, 0, 0);
        /* temparature */
        /*
        input_set_abs_params(akm->input_dev, ABS_THROTTLE, -30, 85, 0, 0);
         */
        /* status of magnetic sensor */
        input_set_abs_params(akm->input_dev, ABS_RUDDER, -32768, 3, 0, 0);
        /* status of acceleration sensor */
        input_set_abs_params(akm->input_dev, ABS_WHEEL, -32768, 3, 0, 0);
        /* x-axis of raw magnetic vector (-4096, 4095) */
        input_set_abs_params(akm->input_dev, ABS_HAT0X, -20480, 20479, 0, 0);
        /* y-axis of raw magnetic vector (-4096, 4095) */
        input_set_abs_params(akm->input_dev, ABS_HAT0Y, -20480, 20479, 0, 0);
        /* z-axis of raw magnetic vector (-4096, 4095) */
        input_set_abs_params(akm->input_dev, ABS_BRAKE, -20480, 20479, 0, 0);
        /* Set name */
        akm->input_dev->name = "compass";
        /* Register */
        err = input_register_device(akm->input_dev);
        if (err) {
                printk(KERN_ERR
                       "kxtf9 init_event: Unable to register input device\n");
                goto exit6;
        }
        return err;
exit8:
exit7:
        input_unregister_device(akm->input_dev);
exit6:
        input_free_device(akm->input_dev);
exit5:
exit4:
exit3:
exit2:
        kfree(akm);
exit1:
exit0:
        return err;
};

int __init s3c_sensor_init(void)
{
    int ret;
    int err = 0;

	/* enable gsensor power. x210ii:GPD0_3 */
	s3c_gpio_setpull(S5PV210_GPD0(3), S3C_GPIO_PULL_UP);
	s3c_gpio_cfgpin(S5PV210_GPD0(3), S3C_GPIO_SFN(1));
	gpio_set_value(S5PV210_GPD0(3), 1);

    sensor_dbg(KERN_INFO "###s3c_sensor_init \n", __FUNCTION__);
    ret = register_chrdev(SENSOR_DEV_MAJOR, SENSOR_DEV_NAME, &s3c_sensor_fops);
    if (ret < 0)
        return ret;

    sensor_class = class_create(THIS_MODULE, SENSOR_DEV_NAME);
    device_create(sensor_class,NULL,MKDEV(SENSOR_DEV_MAJOR,SENSOR_DEV_MINOR),NULL,SENSOR_DEV_NAME);

    INIT_WORK(&sensor_work_q, sensor_fetch_thread);

    init_event();

    err = misc_register(&kxtf9_device);
    if (err) {
	printk(KERN_ERR
	"kxtf9 s3c_sensor_init: kxtf9_device register failed\n");
	goto exit7;
    }
	
	err = misc_register(&kxtf9_aot_device);
	if (err) {
		printk(KERN_ERR
		       "kxtf9 s3c_sensor_init: kxtf9_aot_device register failed\n");
		goto exit8;
	}
#ifdef CONFIG_HAS_EARLYSUSPEND
	early_suspend.suspend = sensor_i2c_suspend;
	early_suspend.resume = sensor_i2c_resume;
	early_suspend.level = 100;
	register_early_suspend(&early_suspend);
#endif

    sensor_dbg(KERN_INFO "%s\n", __FUNCTION__);
exit7:
exit8:
    return 0;
}

void __exit s3c_sensor_exit(void)
{
    sensor_dbg(KERN_INFO "%s\n", __FUNCTION__);
    unregister_chrdev(SENSOR_DEV_MAJOR, SENSOR_DEV_NAME);
}

module_init(s3c_sensor_init);
module_exit(s3c_sensor_exit);

MODULE_AUTHOR("www.9tripod.com");
MODULE_DESCRIPTION("S5PV210 accel-gsensor driver");
MODULE_LICENSE("GPL");

