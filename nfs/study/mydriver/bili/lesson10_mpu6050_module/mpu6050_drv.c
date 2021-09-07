#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/err.h>
//#include <linux/regmap.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/cdev.h>

static ssize_t mpu6050_write ( struct file *file, const char __user *buf, size_t, loff_t *offset );
static ssize_t mpu6050_read ( struct file *file, char __user *buf, size_t size, loff_t *offset );
static int mpu6050drv_probe ( struct i2c_client *i2c_client, const struct i2c_device_id *i2c_device_id );
static int mpu6050drv_remove ( struct i2c_client *i2c_client );

static struct i2c_device_id mpu6050drv_id =
{
    .name = "mpu6050",
    .driver_data = 0x68,

};

static struct i2c_driver mpu6050drv =
{
    .driver    = {
        .name    = "mympu6050",
        .owner    = THIS_MODULE,
    },
    .probe = mpu6050drv_probe,
    .remove = mpu6050drv_remove,
    .id_table = &mpu6050drv_id,

};

/* 字符设备相关 */
int major = 0;
static struct cdev mpu6050_cdev;
static struct file_operations mpu6050ops =
{
    .owner = THIS_MODULE,
    .read = mpu6050_read,
    .write = mpu6050_write,
};

static struct class *cls;
static ssize_t mpu6050_read ( struct file *file, char __user *buf, size_t size, loff_t *offset )
{
    return 0;
}

static ssize_t mpu6050_write ( struct file *file, const char __user *buf, size_t size, loff_t *offset )
{
    return 0;
}
static int mpu6050drv_probe ( struct i2c_client *i2c_client, const struct i2c_device_id *i2c_device_id )
{
    dev_t dev = 0;
    printk ( "mpu6050drv_probe\r\n" );
#if 0
    /* 把mpu6050当做字符设备注册到内核 */
    major = register_chrdev ( 0, "mpu6050", &mpu6050ops );
#else
    alloc_chrdev_region ( &dev, 0, 2, "mpu6050_region" );        // 占用2个次设备号
    cdev_init ( &mpu6050_cdev, &mpu6050ops );
    mpu6050_cdev.owner = THIS_MODULE;
    cdev_add ( &mpu6050_cdev, dev, 2 );                            // 占用2个次设备号

#endif

    cls = class_create ( THIS_MODULE, "mpu6050cls" );
    major = MAJOR ( dev );
    device_create ( cls, NULL, MKDEV ( major, 0 ), NULL, "mpu6050" );
//    device_create ( cls, NULL, MKDEV ( major, 1 ), NULL, "mpu6050_2" );


    return 0;
}
static int mpu6050drv_remove ( struct i2c_client *i2c_client )
{
    printk ( "mpu6050drv_remove\r\n" );
    unregister_chrdev_region ( 0, 1 );
    cdev_del ( &mpu6050_cdev );
    device_destroy ( cls, MKDEV ( major, 0 ) );
//    device_destroy ( cls, MKDEV ( major, 1 ) );
    class_destroy ( cls );
    return 0;
}
static int mpu6050_init ( void )
{
    i2c_add_driver ( &mpu6050drv );
    return 0;
}

static void mpu6050_exit ( void )
{
    i2c_del_driver ( &mpu6050drv );

}

module_init ( mpu6050_init );
module_exit ( mpu6050_exit );
MODULE_LICENSE ( "GPL" );
