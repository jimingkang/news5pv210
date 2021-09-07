#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/err.h>
//#include <linux/regmap.h>
#include <linux/slab.h>

static const unsigned short addr_list[] = { 0x50,0x68, I2C_CLIENT_END };
static struct i2c_board_info mpu6050_info = {
    .type = "mpu6050",
};
static struct i2c_client *mpu6050_client;

static int mpu6050dev_init ( void )
{
    struct i2c_adapter *i2c_adap;
    i2c_adap = i2c_get_adapter(0);
    mpu6050_client = i2c_new_probed_device(i2c_adap,&mpu6050_info,addr_list);
    i2c_put_adapter(i2c_adap);

    if ( mpu6050_client )
        return 0;
    else
        return -ENODEV;
}
static void mpu6050dev_exit(void)
{
    i2c_unregister_device(mpu6050_client);

}

module_init(mpu6050dev_init);
module_exit(mpu6050dev_exit);
MODULE_LICENSE ("GPL");
