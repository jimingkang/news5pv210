#include <linux/module.h>
#include <linux/version.h>

#include <linux/init.h>

#include <linux/fs.h>
#include <linux/cdev.h>
 
#include <linux/slab.h>
#include <linux/ioctl.h>
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
#include "mpu6050.h"

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
static struct cdev mpu6050_cdev;
static struct class *cls;

static struct i2c_client * mpu6050_client;


//jimmy add
#define SMPLRT_DIV	0x19
#define CONFIG		0x1A
#define GYRO_CONFIG	0x1B
#define ACCEL_CONFIG	0x1C
#define ACCEL_XOUT_H	0x3B
#define ACCEL_XOUT_L	0x3C
#define ACCEL_YOUT_H	0x3D
#define ACCEL_YOUT_L	0x3E
#define ACCEL_ZOUT_H	0x3F
#define ACCEL_ZOUT_L	0x40
#define TEMP_OUT_H	0x41
#define TEMP_OUT_L	0x42
#define GYRO_XOUT_H	0x43
#define GYRO_XOUT_L	0x44
#define GYRO_YOUT_H	0x45
#define GYRO_YOUT_L	0x46
#define GYRO_ZOUT_H	0x47
#define GYRO_ZOUT_L	0x48
#define PWR_MGMT_1	0x6B
struct atg_val {
	short accelx;
	short accely;
	short accelz;
	short temp;
	short gyrox;
	short gyroy;
	short gyroz;
};

#define MPU6050_GET_VAL	_IOR(MPU6050_MAGIC, 0, struct atg_val)
#define MPU6050_MAGIC	'm' 
struct mpu6050_dev {
	struct i2c_client *client;
	atomic_t available;
	struct cdev cdev;
	struct device *device;
};

static int mpu6050_read_len(struct i2c_client * client, unsigned char reg_add , unsigned char len, unsigned char *buf)
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
    //return rxbuf[0];
    /*
    // 要读取的那个寄存器的地址 /
    char txbuf = reg_add;

    struct i2c_msg msg[] = {
        {client->addr, 0, 1, &txbuf},       //0表示写，
        {client->addr, I2C_M_RD, len, buf}, //读数据
    };

    // 通过i2c_transfer函数操作msg /
    ret = i2c_transfer(client->adapter, msg, 2);    //执行2条msg
    if (ret < 0)
    {
        printk("i2c_transfer read err\n");
        return -1;
    }

    return 0;
    */
}
int mpu6050_read_byte(
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
/*
static int mpu6050_read_byte(struct i2c_client * client, unsigned char reg_add)
{
    int ret;

    //要读取的那个寄存器的地址 
    char txbuf = reg_add;

    // 用来接收读到的数据 
    char rxbuf[1];

    // i2c_msg指明要操作的从机地址，方向，长度，缓冲区 
    struct i2c_msg msg[] = {
        {client->addr, 0, 1, &txbuf},       //0表示写，
        {client->addr, I2C_M_RD, 1, rxbuf}, //读数据
    };

    // 通过i2c_transfer函数操作msg
    ret = i2c_transfer(client->adapter, msg, 2);    //执行2条msg
    if (ret < 0)
    {
        printk("i2c_transfer read err\n");
        return -1;
    }

    return rxbuf[0];
}
*/

int mpu6050_write_byte( struct i2c_client *client, 
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

/*
static int mpu6050_write_byte(struct i2c_client * client, unsigned char reg_addr, unsigned char data)
{
    int ret;

    // 要写的那个寄存器的地址和要写的数据 
    char txbuf[] = {reg_addr, data};

    struct i2c_msg msg[] = {
        {client->addr, 0, 2, txbuf}//0表示写
    };

    ret = i2c_transfer(client->adapter, msg, 1);
    if (ret < 0)
    {
        printk("i2c_transfer write err\n");
        return -1;
    }

    return 0;
}
*/
static int mpu6050_open(struct inode *inode, struct file *file)   
{
	char res;
	
	printk("%s called\n", __func__);
		
i2c_smbus_write_byte_data(mpu6050_client, MPU_PWR_MGMT1_REG, 0X80);/*复位MPU6050*/
	mdelay(100);
	i2c_smbus_write_byte_data(mpu6050_client, MPU_PWR_MGMT1_REG, 0X00);
    i2c_smbus_write_byte_data(mpu6050_client, MPU_GYRO_CFG_REG, 3<<3);/*陀螺仪传感器,±2000dps*/
    i2c_smbus_write_byte_data(mpu6050_client, MPU_ACCEL_CFG_REG, 0<<3);/*加速度传感器,±2g*/
    i2c_smbus_write_byte_data(mpu6050_client, MPU_SAMPLE_RATE_REG, 1000 /50-1);/*设置采样率50Hz*/
    i2c_smbus_write_byte_data(mpu6050_client, MPU_CFG_REG, 4);/*自动设置LPF为采样率的一半*/
	i2c_smbus_write_byte_data(mpu6050_client, MPU_INT_EN_REG, 0X00);/*关闭所有中断*/
	i2c_smbus_write_byte_data(mpu6050_client, MPU_USER_CTRL_REG, 0X00);/*I2C主模式关闭*/
	i2c_smbus_write_byte_data(mpu6050_client, MPU_FIFO_EN_REG, 0X00);/*关闭FIFO*/
	i2c_smbus_write_byte_data(mpu6050_client, MPU_INTBP_CFG_REG, 0X80);/*INT引脚低电平有效*/
	
	res = i2c_smbus_read_byte_data(mpu6050_client, MPU_DEVICE_ID_REG);
	i2c_smbus_write_byte_data(mpu6050_client, MPU_CFG_REG, 3);//设置数字低通滤波器

printk("I2C ID is =%d ! \n",res);
	if (res == 0x68)//器件ID正确
	{
		printk("I2C ID is right ! \n");
	//	i2c_smbus_write_byte_data(mpu6050_client, MPU_PWR_MGMT1_REG, 0X01);	/*设置CLKSEL,PLL X轴为参考*/
		i2c_smbus_write_byte_data(mpu6050_client, MPU_PWR_MGMT2_REG, 0X00);	/*加速度与陀螺仪都工作*/
		return 0;
	}
	printk("failed !I2C ID is error ! \n");
	return 0;  
}  

static ssize_t mpu6050_read(struct file * file, char __user *buf, size_t count, loff_t *off)
{
	char val;
	unsigned char rxbuf[6], res;
	copy_from_user(&val, buf,1);
	res = mpu6050_read_len(mpu6050_client, MPU_ACCEL_XOUTH_REG, 6 , rxbuf);
	if (res == 0)/* 加速度计原始数据   */
	{
		printk("ax = %d \n", ((u16)rxbuf[0] << 8) | rxbuf[1]);
		printk("ay = %d \n", ((u16)rxbuf[2] << 8) | rxbuf[3]);
		printk("az = %d \n", ((u16)rxbuf[4] << 8) | rxbuf[5]);
	}
	res = mpu6050_read_len(mpu6050_client, MPU_GYRO_XOUTH_REG, 6 , rxbuf);
	if (res == 0)/*陀螺仪原始数据*/
	{
		printk("gx = %d \n", ((u16)rxbuf[0] << 8) | rxbuf[1]);
		printk("gy = %d \n", ((u16)rxbuf[2] << 8) | rxbuf[3]);
		printk("gz = %d \n", ((u16)rxbuf[4] << 8) | rxbuf[5]);
	}
	return 0;
}
static ssize_t mpu6050_write(struct file *file, const char __user *buf, size_t count , loff_t * ppos)
{
	return 0;
}
static long mpu6050_ioctl(struct file *file, unsigned int cmd, unsigned long arg)  
{
	return 0;
}

/* 2. 构造file_operations */
static struct file_operations mpu6050_fops = {
	.owner	= THIS_MODULE,
	.open	= mpu6050_open,
	.read	= mpu6050_read,	   
	.write	= mpu6050_write,
	.unlocked_ioctl   =   mpu6050_ioctl,
};

static int mpu6050_probe(struct i2c_client *client, const struct i2c_device_id *id)  
{
	int res;
	int ret = 0;;
	int version=0;
	struct device *mpu6050_res;
	dev_t devid;

	mpu6050_client = client;
	
	//jimmy add
	struct mpu6050_dev *mpu6050;
	mpu6050 = kzalloc(sizeof(struct mpu6050_dev), GFP_KERNEL);
	if (!mpu6050) {
		ret = -ENOMEM;
		goto cls_err;
	}
	/* 3. 告诉内核 */
#if 0
	major = register_chrdev(0, "hello", &hello_fops); /* (major,  0), (major, 1), ..., (major, 255)都对应hello_fops */
#else /*仅仅是注册设备号*/
	if (major) {
		devid = MKDEV(major, 0);
		register_chrdev_region(devid, 1, "mpu6050");  /* (major,0) 对应 pwm_fops, (major, 1~255)都不对应pwm_fops */
	} else {
		alloc_chrdev_region(&devid, 0, 1, "mpu6050"); /* (major,0) 对应 pwm_fops, (major, 1~255)都不对应pwm_fops */
		major = MAJOR(devid);                     
printk("major=%d,minor=%d\n",major,MINOR(devid));
	}
	
#endif
 
	cdev_init(&mpu6050->cdev, &mpu6050_fops);
mpu6050->cdev.owner = THIS_MODULE;
	res=cdev_add(&mpu6050->cdev, devid, 1);
	if(res)
	{
		printk("res=cdev_add failed\n");
		goto add_err;

	}
	cls = class_create(THIS_MODULE, "mpu6050");
	mpu6050->device = device_create(cls, NULL, MKDEV(major, 0), NULL, "mpu6050"); /* /dev/xxx */
	if (IS_ERR(mpu6050->device)) {
		printk("mpu6050->device = device_create failed\n");
        	ret = PTR_ERR(mpu6050->device);
        	goto add_err;
    	}
	//jimmy
		printk("class_create failed\n");
	i2c_set_clientdata(client, mpu6050);
	mpu6050->client = client;

	
//jimmy add
if (!i2c_check_functionality(client->adapter, I2C_FUNC_SMBUS_WORD_DATA)) {
	printk("i2c_check_functionality failed\n");
		ret = -ENOSYS;
		goto fun_err;
	}
 
	i2c_smbus_write_byte_data(client, PWR_MGMT_1, 0x80);
	msleep(200);
	i2c_smbus_write_byte_data(client, PWR_MGMT_1, 0x40);
	i2c_smbus_write_byte_data(client, PWR_MGMT_1, 0x00);
 
	i2c_smbus_write_byte_data(client, SMPLRT_DIV,   0x7);
	i2c_smbus_write_byte_data(client, CONFIG,       0x6);
	i2c_smbus_write_byte_data(client, GYRO_CONFIG,  0x3 << 3); 
	i2c_smbus_write_byte_data(client, ACCEL_CONFIG, 0x3 << 3);
 
	atomic_set(&mpu6050->available, 1);
 
fun_err:
	cdev_del(&mpu6050->cdev);
add_err:
	kfree(mpu6050);
cls_err:
	class_destroy(cls);
mem_err:
	unregister_chrdev_region(MKDEV(major, 0), 1);

	return ret;
	
}
static int mpu6050_remove(struct i2c_client *client)  
{  
struct mpu6050_dev *mpu6050 = i2c_get_clientdata(client);

 
	cdev_del(&mpu6050->cdev);
	kfree(mpu6050);
unregister_chrdev_region(MKDEV(major, 0), 1);   
	device_unregister(mpu6050->device);
	class_destroy(cls);

	//device_destroy(cls, MKDEV(major, 0));//class_device_destroy(cls,MKDEV(major, 0));

	
	

	
  
    return 0;  
}

static const struct i2c_device_id mpu6050_id[] = {  
    { "mpu6050", 0},  
    {}  
};

struct i2c_driver mpu6050_driver = {  
    .driver = {  
        .name           = "mpu6050",  
        .owner          = THIS_MODULE,  
    },  
    .probe      = mpu6050_probe,  
    .remove     = mpu6050_remove,  
    .id_table   = mpu6050_id,  
};

static int I2C_mpu6050_init(void)
{volatile unsigned long *CLK_GATE_IP3=NULL;
      //      set GPIO pin function as IICSCL, IICSDA
      //GPD1CON[1]  0010 = I2C0_SCL ,GPD1CON[0]  0010 = I2C0_SDA 
      //*GPD1CON &=~0xff;     
    //*GPD1CON |=0x22;
//volatile unsigned long *IOGPD1CON=NULL;
//volatile unsigned long *IOGPD1PUD=NULL;
//volatile unsigned long *IOIICCON=NULL;
//volatile unsigned long *IOIICSTAT=NULL;
 //   IOGPD1CON=ioremap(GPD1CON,4);
  //  IOGPD1PUD=ioremap(GPD1PUD,4);
   //IOIICCON=ioremap(IICCON,4);
  //IOIICSTAT=ioremap(IICSTAT,4);
//	writel(0x22,IOGPD1CON);
//      *GPD1PUD &= ~0xfff;              
 //    *GPD1PUD |= 0xaaa;              //Pull-up enable

//writel(0xaaa,IOGPD1PUD);
    /******Open I2C CLOCK**************/
 //   CLK_GATE_IP3=ioremap(0xE010046C,4);
    //open i2c i2sclock
  //  writel(readl(CLK_GATE_IP3) | (1<<7 |(1 <<4)|1),CLK_GATE_IP3); 
    //iounmap(CLK_GATE_IP3);
    //CLK_GATE_IP3=NULL;
    /**********************************************/
    // Enable ACK, Prescaler IICCLK=PCLK/16, 
    //Enable interrupt, Transmit clock value Tx clock=IICCLK/16
    //writel( (1<<7) | (0<<6) | (1<<5) | (0xe),IOIICCON);
    // Enable IIC bus
    //IIC bus data output enable(Rx/Tx)
   // writel(readl(IOIICSTAT)|0x10,IOIICSTAT);
    //iounmap(IOGPD1CON);
    //iounmap(IOGPD1PUD);
    //iounmap(IOIICCON);
    //iounmap(IOIICSTAT);
	return i2c_add_driver(&mpu6050_driver);
}

static void I2C_mpu6050_exit(void)
{
	return i2c_del_driver(&mpu6050_driver);
}

module_init(I2C_mpu6050_init);
module_exit(I2C_mpu6050_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kite");/*modinfo my_keyboard.ko*/
MODULE_DESCRIPTION("A i2c-mpu6050 Module for testing module ");
MODULE_VERSION("V1.0");
