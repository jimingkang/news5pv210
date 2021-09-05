#include <linux/module.h>
#include <linux/version.h>

#include <linux/init.h>
//jimmy add
#include <linux/gpio.h>
#include <asm/io.h>
#include <mach/irqs.h>		// arch/arm/mach-s5pv210/include/mach/irqs.h
#include <plat/gpio-cfg.h>
#include <mach/regs-gpio.h>
#include <mach/gpio.h>
#include <mach/gpio-bank.h>
#include <linux/io.h>
#include <linux/ioport.h>

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/list.h>
#include <linux/timer.h>
#include <linux/init.h>
#include <linux/serial_core.h>
#include <linux/platform_device.h>
#include <linux/uaccess.h>
#include <asm/mach/map.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include "pca9685.h"

//#define GPD1CON S5PV210_GPD1CON
//#define GPD1PUD S5PV210_GPD1PUD
//#define IICCON S5PV210_IICCON0
//#define IICSTAT S5PV210_IICSTAT0

#define GPD1CON *((volatile unsigned int*)0xE02000C0)
#define GPD1PUD *((volatile unsigned int*)0xE02000C8)
#define IICCON *((volatile unsigned int*)0xE1800000)
#define IICSTAT *((volatile unsigned int*)0xE1800004)

/* 1. 确定主设备号 */
static int major;
static struct cdev pca9685_cdev;
static struct class *cls;

static struct i2c_client * pca9685_client;

bool initialized = false;

void initPCA9685():  {
        pca9685_write_byte(pca9685_client, MODE1, 0x00)
        setFreq(50);
        setPwm(0, 0, 4095);
        for (int idx = 1; idx < 16; idx++) {
            setPwm(idx, 0, 0);
        }
        initialized = true;
    }
void setFreq(long freq)  {
        // Constrain the frequency
        long prescaleval = 25000000;
		 long prescale = 0; //Math.Floor(prescaleval + 0.5);
        long oldmode = 0;
        long newmode = 0; // sleep
        prescaleval /= 4096;
        prescaleval /= freq;
        prescaleval -= 1;
         prescale = prescaleval; //Math.Floor(prescaleval + 0.5);
         oldmode = i2cread(pca9685_client, MODE1);
         newmode = (oldmode & 0x7F) | 0x10; // sleep
        pca9685_write_byte(pca9685_client, MODE1, newmode); // go to sleep
        pca9685_write_byte(pca9685_client, PRESCALE, prescale); // set the prescaler
        pca9685_write_byte(pca9685_client, MODE1, oldmode);
        mdelay(5000);
        pca9685_write_byte(pca9685_client, MODE1, oldmode | 0xa1);
    }
 
    int setPwm(int channel, int on, int off)  {
		
		 unsigned int buf[5]; // let buf = pins.createBuffer(5);
		  struct i2c_msg msg[1];
    uint8_t txbuf[] = {reg_addr, data};
    int ret;
        if (channel < 0 || channel > 15)
            return;
 
        buf[0] = LED0_ON_L + 4 * channel;
        buf[1] = on & 0xff;
        buf[2] = (on >> 8) & 0xff;
        buf[3] = off & 0xff;
        buf[4] = (off >> 8) & 0xff;
       
	   

    memset(msg, 0, sizeof(msg));
    msg[0].addr = client->addr;
    msg[0].buf = buf;
    msg[0].len = sizeof(buf);

    ret = i2c_transfer(pca9685_client->adapter, msg, ARRAY_SIZE(msg));
	return ret;
    }
 
 
 void Servo(int channel,int degree) {
	         float v_us =0; // 0.6 ~ 2.4
        int value =0;
        if (!initialized) {
            initPCA9685();
        }
        // 50hz: 20,000 us
         v_us = (degree * 1800 / 180 + 600); // 0.6 ~ 2.4
         value = v_us * 4096 / 20000;
        setPwm(channel, 0, value);
    }
	void ServoPulse(int channel,int pulse):  {
		 int value=0;
        if (!initialized) {
            initPCA9685();
        }
        // 50hz: 20,000 us
         value = pulse * 4096 / 20000;
        setPwm(channel, 0, value);
    }
static int pca9685_read_len(struct i2c_client * client, unsigned char reg_add , unsigned char len, unsigned char *buf)
{
    int ret;
    
    struct i2c_msg msgs[2];
    uint8_t txbuf = reg_add;
    memset(msgs, 0, sizeof(msgs));
    msgs[0].addr = client->addr;
    msgs[0].buf = &txbuf;
    msgs[0].len = 1;

    msgs[1].addr = client->addr;
    msgs[1].flags = I2C_M_RD|I2C_M_IGNORE_NAK;
    msgs[1].buf = buf;
    msgs[1].len = len;

    ret = i2c_transfer(client->adapter, msgs, ARRAY_SIZE(msgs));
    if (ret < 0) {
        pr_info("%s: fail to transfer\n", __func__);
        return -EFAULT;
    }
    printk("1 = %2x %2x\n", buf[0], buf[1]);
return 0;

}


int pca9685_read_byte(
            struct i2c_client *client, uint8_t reg_addr)
{
    struct i2c_msg msgs[2];
    uint8_t txbuf = reg_addr;
    int ret;
int len=1; 
    char rxbuf[1];
    memset(msgs, 0, sizeof(msgs));
    msgs[0].addr = client->addr;
    msgs[0].buf = &txbuf;
    msgs[0].len = 1;

    msgs[1].addr = client->addr;
    msgs[1].flags = I2C_M_RD|I2C_M_IGNORE_NAK;
    msgs[1].buf = rxbuf;
    msgs[1].len = len;

    ret = i2c_transfer(client->adapter, msgs, ARRAY_SIZE(msgs));
    if (ret < 0) {
        pr_info("%s: fail to transfer\n", __func__);
        return -EFAULT;
    }

    return rxbuf[0];
}

int pca9685_write_byte( struct i2c_client *client, 
            uint8_t reg_addr,uint8_t data)
{
    struct i2c_msg msg[1];
    uint8_t txbuf[] = {reg_addr, data};
    int ret;

    memset(msg, 0, sizeof(msg));
    msg[0].addr = client->addr;
    msg[0].buf = txbuf;
    msg[0].len = sizeof(txbuf);

    ret = i2c_transfer(client->adapter, msg, ARRAY_SIZE(msg));
    if (ret < 0) {
        pr_info("%s: fail to transfer\n", __func__);
        return -EFAULT;
    }

    return 0;
}

static int pca9685_open(struct inode *inode, struct file *file)   
{
	char res;
	
	printk("%s called\n", __func__);
		
initPCA9685();


	return 0;  
}  

static ssize_t pca9685_read(struct file * file, char __user *buf, size_t count, loff_t *off)
{
	char val;
	unsigned char rxbuf[12], res;
	//res = pca9685_read_len(pca9685_client, MPU_ACCEL_XOUTH_REG, 6 , rxbuf);
	if (res == 0)/* 加速度计原始数据   */
	{
	//	printk("ax = %d \n", ((u16)rxbuf[0] << 8) | rxbuf[1]);
	//	printk("ay = %d \n", ((u16)rxbuf[2] << 8) | rxbuf[3]);
	//	printk("az = %d \n", ((u16)rxbuf[4] << 8) | rxbuf[5]);
	}
	//res = pca9685_read_len(pca9685_client, MPU_GYRO_XOUTH_REG, 6 , rxbuf);
	if (res == 0)/*陀螺仪原始数据*/
	{
	//	printk("gx = %d \n", ((u16)rxbuf[6+0] << 8) | rxbuf[6+1]);
	//	printk("gy = %d \n", ((u16)rxbuf[6+2] << 8) | rxbuf[6+3]);
//		printk("gz = %d \n", ((u16)rxbuf[6+4] << 8) | rxbuf[6+5]);
	}
	copy_to_user(buf, rxbuf,12);
	return 0;
}
static ssize_t pca9685_write(struct file *file, const char __user *buf, size_t count , loff_t * ppos)
{
	return 0;
}
static long pca9685_ioctl(struct file *file, unsigned int cmd, unsigned long arg)  
{
	return 0;
}

/* 2. 构造file_operations */
static struct file_operations pca9685_fops = {
	.owner	= THIS_MODULE,
	.open	= pca9685_open,
	.read	= pca9685_read,	   
	.write	= pca9685_write,
	.unlocked_ioctl   =   pca9685_ioctl,
};

static int pca9685_probe(struct i2c_client *client, const struct i2c_device_id *id)  
{
	int res;
	int version=0;
	int version2=0;
	struct device *pca9685_res;
	dev_t devid;

	pca9685_client = client;
	printk("probe   client %s，\n", pca9685_client->name);
//read_reg(pca9685_client, MPU_PWR_MGMT1_REG, 0X80);/*复位MPU6050*/
//	mdelay(100);
//	i2c_smbus_write_byte_data(pca9685_client, MPU_PWR_MGMT1_REG, 0X00);
//version = i2c_smbus_read_byte_data(client,0x75);
//version2=read_reg(client,0x75);
//printk("probe   version %d，version2=%d\n", version,version2);

	/* 3. 告诉内核 */
#if 0
	major = register_chrdev(0, "hello", &hello_fops); /* (major,  0), (major, 1), ..., (major, 255)都对应hello_fops */
#else /*仅仅是注册设备号*/
	if (major) {
		devid = MKDEV(major, 0);
		register_chrdev_region(devid, 1, "pca9685");  /* (major,0) 对应 pwm_fops, (major, 1~255)都不对应pwm_fops */
	} else {
		alloc_chrdev_region(&devid, 0, 1, "pca9685"); /* (major,0) 对应 pwm_fops, (major, 1~255)都不对应pwm_fops */
		major = MAJOR(devid);                     
printk("major=%d,minor=%d\n",major,MINOR(devid));
	}
	
	cdev_init(&pca9685_cdev, &pca9685_fops);
	res=cdev_add(&pca9685_cdev, devid, 1);
	if(res)
	{
		printk("cdev_add failed\n");
		unregister_chrdev_region(MKDEV(major, 0), 1);
		return 0;
	}
#endif
	cls = class_create(THIS_MODULE, "pca9685");
	pca9685_res = device_create(cls, NULL, MKDEV(major, 0), NULL, "pca9685"); /* /dev/xxx */
	if (IS_ERR(pca9685_res)) 
	{
		printk("device_create failed\n");
		return 0;
	}

	return 0;	
}
static int pca9685_remove(struct i2c_client *client)  
{  
	device_destroy(cls, MKDEV(major, 0));//class_device_destroy(cls,MKDEV(major, 0));

	class_destroy(cls);
	
	cdev_del(&pca9685_cdev);
	unregister_chrdev_region(MKDEV(major, 0), 1);   
  
    return 0;  
}

static const struct i2c_device_id pca9685_id[] = {  
    { "pca9685", 0},  
    {}  
};

struct i2c_driver pca9685_driver = {  
    .driver = {  
        .name           = "pca9685",  
        .owner          = THIS_MODULE,  
    },  
    .probe      = pca9685_probe,  
    .remove     = pca9685_remove,  
    .id_table   = pca9685_id,  
};

static int I2C_pca9685_init(void)
{volatile unsigned long *CLK_GATE_IP3=NULL;
      
	return i2c_add_driver(&pca9685_driver);
}

static void I2C_pca9685_exit(void)
{
	return i2c_del_driver(&pca9685_driver);
}

module_init(I2C_pca9685_init);
module_exit(I2C_pca9685_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kite");/*modinfo my_keyboard.ko*/
MODULE_DESCRIPTION("A i2c-pca9685 Module for testing module ");
MODULE_VERSION("V1.0");
