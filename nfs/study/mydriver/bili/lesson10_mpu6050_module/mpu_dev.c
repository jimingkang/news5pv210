#include <linux/module.h>
#include <linux/version.h>

#include <linux/init.h>

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/list.h>
#include <linux/timer.h>
#include <linux/init.h>
#include <linux/serial_core.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>


static struct i2c_board_info mpu6050_info = {	
	I2C_BOARD_INFO("mpu6050", 0X68),//接地为0X68 接高电平为0X69
};

static struct i2c_client *mpu6050_client;

static int I2C_mpu6050_init(void)
{
	struct i2c_adapter *i2c_adap;

	i2c_adap = i2c_get_adapter(0);
	mpu6050_client = i2c_new_device(i2c_adap, &mpu6050_info);
	i2c_put_adapter(i2c_adap);
	
	return 0;
}

static void I2C_mpu6050_exit(void)
{
	i2c_unregister_device(mpu6050_client);
}

module_init(I2C_mpu6050_init);
module_exit(I2C_mpu6050_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kite");/*modinfo my_keyboard.ko*/
MODULE_DESCRIPTION("A mpu6050 Module for testing module ");
MODULE_VERSION("V1.0");
