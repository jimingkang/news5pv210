#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/miscdevice.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#include "mpu6050.h"

#define I2C_0 0     // i2c-0 bus number
#define DEV_NAME "mpu6050"

struct mpu6050_i2c_dev {
    struct miscdevice miscdev;
    struct i2c_client *client;
};

int mpu6050_read_bytes(uint8_t *rxbuf, uint8_t len, 
            struct i2c_client *client, uint8_t reg_addr)
{
    struct i2c_msg msgs[2];
    uint8_t txbuf = reg_addr;
    int ret;

    memset(msgs, 0, sizeof(msgs));
    msgs[0].addr = client->addr;
    msgs[0].buf = &txbuf;
    msgs[0].len = 1;

    msgs[1].addr = client->addr;
    msgs[1].flags = I2C_M_RD;
    msgs[1].buf = rxbuf;
    msgs[1].len = len;

    ret = i2c_transfer(client->adapter, msgs, ARRAY_SIZE(msgs));
    if (ret < 0) {
        pr_info("%s: fail to transfer\n", __func__);
        return -EFAULT;
    }

    return 0;
}

int mpu6050_read_byte(uint8_t *data, struct i2c_client *client, 
            uint8_t reg_addr)
{
    return mpu6050_read_bytes(data, 1, client, reg_addr);
}

int mpu6050_write_byte(uint8_t data, struct i2c_client *client, 
            uint8_t reg_addr)
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

int mpu6050_test_connect(struct i2c_client *client)
{
    uint8_t slave_addr;
    int ret;

    ret = mpu6050_read_byte(&slave_addr, client, 0x75);
    if (ret < 0)
        return -EFAULT;

    if (slave_addr != client->addr) {
        pr_info("%s: %d isn't client address\n", __func__, slave_addr);
        return -ENODEV;
    }
    pr_info("%s: mpu6050 connected\n", __func__);

    return 0;
}

ssize_t mpu6050_i2c_read(struct file *filp, char __user *buf, 
                size_t count, loff_t *ppos)
{
    // The miscdevice pointer is automatically placed in private_data when misc is registered, this is
    // Why use miscdevice entities instead of pointers in i2c_memery_dev
    struct mpu6050_i2c_dev *dev = container_of(filp->private_data,
struct mpu6050_i2c_dev, miscdev);
    uint8_t tmp[14];
    int ret;

    if (count > 14) {
        pr_info("count > 14");
        count = 14;
    } else if (count < 1) {
        pr_info("count < 1");
        count = 1;
    }
    ret = mpu6050_read_bytes(tmp, 14, dev->client, 
                0x3B);
    if (ret < 0) 
        goto out;

    pr_info("%d %d %d, %d %d %d\n", tmp[0]<<8 | tmp[1], 
            tmp[2]<<8 | tmp[3], tmp[4]<<8 | tmp[5], tmp[8]<<8 | tmp[9],
            tmp[10]<<8 | tmp[11], tmp[12]<<8 | tmp[13]);
    if (copy_to_user(buf, tmp, count))
        ret = -EFAULT;
    else 
        ret = count;
out:    
    return ret;
}

ssize_t mpu6050_i2c_write (struct file *filp, const char __user *buf,
            size_t count, loff_t *ppos)
{
    struct mpu6050_i2c_dev *dev = container_of(filp->private_data,
                    struct mpu6050_i2c_dev, miscdev);

    return 0;
}

static int mpu6050_i2c_release(struct inode *inode, struct file *filp)
{ 
    pr_info("%s\n", __func__);
    return 0;
}

static int mpu6050_i2c_open(struct inode *inode, struct file *filp)
{
    struct mpu6050_i2c_dev *dev = container_of(filp->private_data,
                    struct mpu6050_i2c_dev, miscdev);
    int ret;

    ret = mpu6050_test_connect(dev->client);
    if (ret < 0)
        return ret;
    pr_info("%s: Use default configuration, Low pass filter level 6,"
            "Gyro range: +-2000 deg/s, Acceleration range: +-2 g\n"
            , __func__);
    mpu6050_write_byte(0x07, dev->client, 1000/50-1);
    mpu6050_write_byte(0x06, dev->client, 4);
    mpu6050_write_byte(0x18, dev->client, 3<<3);
    mpu6050_write_byte(0x07, dev->client, 0<<3);
    mpu6050_write_byte(0x00, dev->client, 0x00);

    return 0;
}

static struct file_operations mpu6050_i2c_fops = {
	.owner	= THIS_MODULE,
    .write = mpu6050_i2c_write,
    .read = mpu6050_i2c_read,
    .open = mpu6050_i2c_open,
    .release = mpu6050_i2c_release, 
};

static int mpu6050_i2c_probe(struct i2c_client *client, 
                const struct i2c_device_id *id)
{
    struct mpu6050_i2c_dev *dev;
    int ret;

    dev = kzalloc(sizeof(*dev), GFP_KERNEL);
    if (!dev) {
        pr_info("%s: fail to malloc memery\n", __func__);
        return -ENOMEM;
    }

    // Set the driver of the spurious device
    dev->miscdev.minor = MISC_DYNAMIC_MINOR;
    dev->miscdev.name = "mpu6050_i2c";
    dev->miscdev.fops = &mpu6050_i2c_fops;
    // Set private data
    dev->client = client;
    dev_set_drvdata(&client->dev, dev);

    ret = misc_register(&dev->miscdev);
    if (ret < 0) {
        return ret;
    }

    printk(KERN_INFO "%s\n", __func__);

    return 0;
}

static int mpu6050_i2c_remove(struct i2c_client *client)
{
    struct mpu6050_i2c_dev *dev = dev_get_drvdata(&client->dev);
    misc_deregister(&dev->miscdev);
    return 0;
}

// The client here is temporary and not registered to the system
// Because adapter.class = I2C_CLASS_DEPRECATED here, automatic detection is not supported
static int mpu6050_i2c_detect(struct i2c_client *client, 
                struct i2c_board_info *info)
{
    struct i2c_adapter *adap = client->adapter;
    int ret;

    printk(KERN_INFO "mpu6050 detect\n");

    if (!i2c_check_functionality(adap, I2C_FUNC_SMBUS_BYTE_DATA))
        return -ENODEV;

    if (I2C_0 == adap->nr) {
        pr_info("detect addr = %x:\n", client->addr);
        // Detect all possible addresses in i2c_driver->address_list
        ret = mpu6050_test_connect(client);
        if (!ret) {
            pr_info("fail to find device\n");
            return -ENODEV;
        } else {
            // This info->type is the name of the client. Assign it to it and the system will create it
            // A new i2c_client to the system
            strcpy(info->type, DEV_NAME);
            return 0;
        }
    } else {
        printk(KERN_INFO "not i2c-0");
    }

    return -ENODEV;
}

static const struct i2c_device_id mpu6050_i2c_ids[] = {
    {DEV_NAME, 0x68}
};

const unsigned short mpu6050_addr_list[] = {
    0x68,
    0x69,
0x70,
};

static struct i2c_driver mpu6050_i2c_driver = {
    .driver = {
        .name = "mpu6050",
        .owner = THIS_MODULE,
    },
    .probe = mpu6050_i2c_probe,
    .remove = mpu6050_i2c_remove,
    .detect = mpu6050_i2c_detect,
    .id_table = mpu6050_i2c_ids,
    .address_list = mpu6050_addr_list,
};


static struct i2c_board_info mpu6050_i2c_info = {
    .type = DEV_NAME,
    .addr =0x68 
};


static struct i2c_client *temp_client;
static int __init mpu6050_i2c_init(void)
{
    struct i2c_adapter *adapter;

    adapter = i2c_get_adapter(I2C_0);
    if (!adapter)
        printk(KERN_INFO "fail to get i2c-%d\n", I2C_0);

    // Temporarily register an i2c_client
    temp_client = i2c_new_device(adapter, &mpu6050_i2c_info);
    if (!temp_client)
        printk(KERN_INFO "fail to registe %s\n", mpu6050_i2c_info.type);

    pr_info(KERN_INFO "mpu6050 i2c init\n");
    return i2c_add_driver(&mpu6050_i2c_driver);
}
module_init(mpu6050_i2c_init);

static void __exit mpu6050_i2c_exit(void)
{
    i2c_unregister_device(temp_client);
    i2c_del_driver(&mpu6050_i2c_driver);
}
module_exit(mpu6050_i2c_exit);

MODULE_AUTHOR("colourfate <hzy1q84@foxmail.com>");
MODULE_LICENSE("GPL v2");
