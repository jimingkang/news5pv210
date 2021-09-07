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


/* 1. 确定主设备号 */
static int major;
static struct cdev PCA9685_cdev;
static struct class *cls;

static struct i2c_client * PCA9685_client;

bool initialized = false;
static int iic_write( struct i2c_client* client,uint8_t reg,uint8_t data)
{
unsigned char buffer[2];
buffer[0] = reg;
buffer[1] = data;
if( 2!= i2c_master_send(client,buffer,2) ) {
printk( KERN_ERR " i2c_write fail! \n" );
return -1;
}
return 0;
}

static int i2c_read( struct i2c_client* client,uint8_t reg,uint8_t *data)
{
// write reg addr
if( 1!= i2c_master_send(client,&reg,1) ) {
printk( KERN_ERR " tvp5158_i2c_read fail! \n" );
return -1;
}
// wait
msleep(10);
// read
if( 1!= i2c_master_recv(client,data,1) ) {
printk( KERN_ERR "i2c_read fail! \n" );
return -1;
}
return 0;
}


static int PCA9685_read_len(struct i2c_client * client, unsigned char reg_add , unsigned char len, unsigned char *buf)
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


int PCA9685_read_byte(
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

int PCA9685_write_byte( struct i2c_client *client, 
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

void setFreq(long freq)  {
        // Constrain the frequency
        int prescaleval = 25000000;
		 int prescale = 0; //Math.Floor(prescaleval + 0.5);
        int oldmode = 0;
        int newmode = 0; // sleep
        prescaleval /= 4096;
        prescaleval /= freq;
        prescaleval -= 1;
         prescale =(char)prescaleval; //Math.Floor(prescaleval + 0.5);
         oldmode = PCA9685_read_byte(PCA9685_client, MODE1);
 printk("oldmode :%d,prescaleval =%d\n",oldmode,prescaleval );

         newmode = (oldmode & 0x7F) | 0x10; // sleep
        PCA9685_write_byte(PCA9685_client, MODE1, newmode); // go to sleep
        PCA9685_write_byte(PCA9685_client, PRESCALE, prescale); // set the prescaler
        PCA9685_write_byte(PCA9685_client, MODE1, oldmode);
        mdelay(5000);
        PCA9685_write_byte(PCA9685_client, MODE1, oldmode | 0xa1);
 printk("setFreq done\n");
    }
 
    int setPwm(int channel, unsigned int on, unsigned int off)  {
		
		 unsigned int buf[5]; // let buf = pins.createBuffer(5);
		  struct i2c_msg msg[1];
    //uint8_t txbuf[] = {reg_addr, data};
    int ret;
        if (channel < 0 || channel > 15)
            return -1;
 
        buf[0] =  4 * channel;
        buf[1] = on & 0xff;
        buf[2] = (on >> 8) & 0xff;
        buf[3] = off & 0xff;
        buf[4] = (off >> 8) & 0xff;
       
	   

    memset(msg, 0, sizeof(msg));
    msg[0].addr = PCA9685_client->addr;
    msg[0].buf = buf;
    msg[0].len = sizeof(buf);

    ret = i2c_transfer(PCA9685_client->adapter, msg, ARRAY_SIZE(msg));
 printk("setPwm is that OK?\n");
	return ret;
    }
 
void initPCA9685(){
	int idx=0;
        PCA9685_write_byte(PCA9685_client,MODE1,0x00);
        setFreq(50);
        setPwm(0, 0,4095);
        for ( idx = 1; idx < 16; idx++) {
            setPwm(idx, 0, 4095);
        }
        initialized = true;
    }
 
 void Servo(int channel,int degree) {
	   unsigned int v_us =0; // 0.6 ~ 2.4
        unsigned int value =0;
        if (!initialized) {
            initPCA9685();
        }
        // 50hz: 20,000 us
         v_us = (degree * 1800 / 180 + 600); // 0.6 ~ 2.4
         value = v_us * 4096 / 20000;
        setPwm(channel, 0, value);
    }
void ServoPulse(int channel,int pulse){
		 unsigned int value=0;
        if (!initialized) {
            initPCA9685();
        }
        // 50hz: 20,000 us
         value = pulse * 4096 / 20000;
        setPwm(channel, 0, value);
}

static int PCA9685_open(struct inode *inode, struct file *file)   
{
	char res;
	
	printk("%s called\n", __func__);
		
initPCA9685();


	return 0;  
}  

static ssize_t PCA9685_read(struct file * file, char __user *buf, size_t count, loff_t *off)
{
	char val;
	unsigned char rxbuf[12], res;
	//res = PCA9685_read_len(PCA9685_client, MPU_ACCEL_XOUTH_REG, 6 , rxbuf);
	if (res == 0)/* 加速度计原始数据   */
	{
	//	printk("ax = %d \n", ((u16)rxbuf[0] << 8) | rxbuf[1]);
	//	printk("ay = %d \n", ((u16)rxbuf[2] << 8) | rxbuf[3]);
	//	printk("az = %d \n", ((u16)rxbuf[4] << 8) | rxbuf[5]);
	}
	//res = PCA9685_read_len(PCA9685_client, MPU_GYRO_XOUTH_REG, 6 , rxbuf);
	if (res == 0)/*陀螺仪原始数据*/
	{
	//	printk("gx = %d \n", ((u16)rxbuf[6+0] << 8) | rxbuf[6+1]);
	//	printk("gy = %d \n", ((u16)rxbuf[6+2] << 8) | rxbuf[6+3]);
//		printk("gz = %d \n", ((u16)rxbuf[6+4] << 8) | rxbuf[6+5]);
	}
	copy_to_user(buf, rxbuf,12);
	return 0;
}
static ssize_t PCA9685_write(struct file *file, const char __user *buf, size_t count , loff_t * ppos)
{
	return 0;
}
static long PCA9685_ioctl(struct file *file, unsigned int cmd, unsigned long arg)  
{
	return 0;
}

/* 2. 构造file_operations */
static struct file_operations PCA9685_fops = {
	.owner	= THIS_MODULE,
	.open	= PCA9685_open,
	.read	= PCA9685_read,	   
	.write	= PCA9685_write,
	.unlocked_ioctl   =   PCA9685_ioctl,
};

static int PCA9685_probe(struct i2c_client *client, const struct i2c_device_id *id)  
{
	int res;
	int version=0;
	int version2=0;
	struct device *PCA9685_res;
	dev_t devid;

	PCA9685_client = client;
	printk("probe   client %s，\n", PCA9685_client->name);
	/* 3. 告诉内核 */
#if 0
	major = register_chrdev(0, "hello", &hello_fops); /* (major,  0), (major, 1), ..., (major, 255)都对应hello_fops */
#else /*仅仅是注册设备号*/
	if (major) {
		devid = MKDEV(major, 0);
		register_chrdev_region(devid, 1, "PCA9685");  /* (major,0) 对应 pwm_fops, (major, 1~255)都不对应pwm_fops */
	} else {
		alloc_chrdev_region(&devid, 0, 1, "PCA9685"); /* (major,0) 对应 pwm_fops, (major, 1~255)都不对应pwm_fops */
		major = MAJOR(devid);                     
printk("major=%d,minor=%d\n",major,MINOR(devid));
	}
	
	cdev_init(&PCA9685_cdev, &PCA9685_fops);
	res=cdev_add(&PCA9685_cdev, devid, 1);
	if(res)
	{
		printk("cdev_add failed\n");
		unregister_chrdev_region(MKDEV(major, 0), 1);
		return 0;
	}
#endif
	cls = class_create(THIS_MODULE, "PCA9685");
	PCA9685_res = device_create(cls, NULL, MKDEV(major, 0), NULL, "PCA9685"); /* /dev/xxx */
	if (IS_ERR(PCA9685_res)) 
	{
		printk("device_create failed\n");
		return 0;
	}

	return 0;	
}
static int PCA9685_remove(struct i2c_client *client)  
{  
	device_destroy(cls, MKDEV(major, 0));//class_device_destroy(cls,MKDEV(major, 0));

	class_destroy(cls);
	
	cdev_del(&PCA9685_cdev);
	unregister_chrdev_region(MKDEV(major, 0), 1);   
  
    return 0;  
}

static const struct i2c_device_id PCA9685_id[] = {  
    { "PCA9685", 0},  
    {}  
};

struct i2c_driver PCA9685_driver = {  
    .driver = {  
        .name           = "PCA9685",  
        .owner          = THIS_MODULE,  
    },  
    .probe      = PCA9685_probe,  
    .remove     = PCA9685_remove,  
    .id_table   = PCA9685_id,  
};

static int I2C_PCA9685_init(void)
{volatile unsigned long *CLK_GATE_IP3=NULL;
      
	return i2c_add_driver(&PCA9685_driver);
}

static void I2C_PCA9685_exit(void)
{
	return i2c_del_driver(&PCA9685_driver);
}

module_init(I2C_PCA9685_init);
module_exit(I2C_PCA9685_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kite");/*modinfo my_keyboard.ko*/
MODULE_DESCRIPTION("A i2c-PCA9685 Module for testing module ");
MODULE_VERSION("V1.0");
