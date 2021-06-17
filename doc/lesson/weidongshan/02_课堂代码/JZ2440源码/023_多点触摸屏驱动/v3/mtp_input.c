#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/input.h>
#include <linux/irq.h>
#include <asm/mach/irq.h>

#include <linux/gpio.h>
#include <mach/gpio.h>
#include <plat/gpio-cfg.h>


#define MTP_ADDR   (0x70 >> 1)
#define MTP_MAX_X  800
#define MTP_MAX_Y  480

#define MTP_IRQ  gpio_to_irq(EXYNOS4_GPX1(6))

#define MTP_NAME "ft5x0x_ts"
#define MTP_MAX_ID 15 /* 由硬件决定 */

struct input_dev *ts_dev;
static struct work_struct mtp_work;
static struct i2c_client *mtp_client;

struct mtp_event {
	int x;
	int y;
	int id;
};

static struct mtp_event mtp_events[16];
static int mtp_points;


static irqreturn_t mtp_interrupt(int irq, void *dev_id) {

	/* 本该:
	 * 获取触点数据, 并上报
	 * 但是I2C是慢速设备, 不该放在中断服务程序中操作
	 */

	/* 使用工作队列, 让内核线程来操作 */
	schedule_work(&mtp_work);

	return IRQ_HANDLED;
}


static int mtp_ft5x0x_i2c_rxdata(struct i2c_client *client, char *rxdata, int length) {
	int ret;
	struct i2c_msg msgs[] = {
		{
			.addr	= client->addr,
			.flags	= 0,
			.len	= 1,
			.buf	= rxdata,
		},
		{
			.addr	= client->addr,
			.flags	= I2C_M_RD,
			.len	= length,
			.buf	= rxdata,
		},
	};

	ret = i2c_transfer(client->adapter, msgs, 2);
	if (ret < 0)
		pr_err("%s: i2c read error: %d\n", __func__, ret);

	return ret;
}

static int mtp_ft5x0x_read_data(void) {
	u8 buf[32] = { 0 };
	int ret;

	ret = mtp_ft5x0x_i2c_rxdata(mtp_client, buf, 31);

	if (ret < 0) {
		printk("%s: read touch data failed, %d\n", __func__, ret);
		return ret;
	}

	mtp_points = buf[2] & 0x0f;

	switch (mtp_points) {
		case 5:
			mtp_events[4].x = (s16)(buf[0x1b] & 0x0F)<<8 | (s16)buf[0x1c];
			mtp_events[4].y = (s16)(buf[0x1d] & 0x0F)<<8 | (s16)buf[0x1e];
			mtp_events[4].id = buf[0x1d]>>4;
		case 4:
			mtp_events[3].x = (s16)(buf[0x15] & 0x0F)<<8 | (s16)buf[0x16];
			mtp_events[3].y = (s16)(buf[0x17] & 0x0F)<<8 | (s16)buf[0x18];
			mtp_events[3].id = buf[0x17]>>4;
		case 3:
			mtp_events[2].x = (s16)(buf[0x0f] & 0x0F)<<8 | (s16)buf[0x10];
			mtp_events[2].y = (s16)(buf[0x11] & 0x0F)<<8 | (s16)buf[0x12];
			mtp_events[2].id = buf[0x11]>>4;
		case 2:
			mtp_events[1].x = (s16)(buf[0x09] & 0x0F)<<8 | (s16)buf[0x0a];
			mtp_events[1].y = (s16)(buf[0x0b] & 0x0F)<<8 | (s16)buf[0x0c];
			mtp_events[1].id = buf[0x0b]>>4;
		case 1:
			mtp_events[0].x = (s16)(buf[0x03] & 0x0F)<<8 | (s16)buf[0x04];
			mtp_events[0].y = (s16)(buf[0x05] & 0x0F)<<8 | (s16)buf[0x06];
			mtp_events[0].id = buf[0x05]>>4;
			break;
		case 0:	
			return 0;
		default:
			//printk("%s: invalid touch data, %d\n", __func__, event->touch_point);
			return -1;
	}

	return 0;
}


static void mtp_work_func(struct work_struct *work)
{
	int i;
	int ret;
	
	/* 读取I2C设备, 获得触点数据, 并上报 */
	/* 读取 */
	ret = mtp_ft5x0x_read_data();
	if (ret < 0)
		return;

	/* 上报 */
	if (!mtp_points) {
		input_mt_sync(ts_dev);
		input_sync(ts_dev);
		return;
	}
	
	for (i = 0; i < mtp_points; i++) { /* 每一个点 */
	    input_report_abs(ts_dev, ABS_MT_POSITION_X, mtp_events[i].x);
	    input_report_abs(ts_dev, ABS_MT_POSITION_Y, mtp_events[i].y);
	    input_report_abs(ts_dev, ABS_MT_TRACKING_ID, mtp_events[i].id);
	    input_mt_sync(ts_dev);	
	}
	input_sync(ts_dev);
}

static int __devinit mtp_probe(struct i2c_client *client,
				  const struct i2c_device_id *id)
{
	printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

	/* 终于进入了输入子系统 */
	mtp_client = client;

	/* 分配input_dev */
	ts_dev = input_allocate_device();

	/* 设置 */
	/* 2.1 能产生哪类事件 */
	set_bit(EV_SYN, ts_dev->evbit);
	set_bit(EV_ABS, ts_dev->evbit);

	/* 2.2 能产生这类事件中的哪些 */
	set_bit(ABS_MT_TRACKING_ID, ts_dev->absbit);
	set_bit(ABS_MT_POSITION_X,  ts_dev->absbit);
	set_bit(ABS_MT_POSITION_Y,  ts_dev->absbit);

	/* 2.3 这些事件的范围 */
	input_set_abs_params(ts_dev, ABS_MT_TRACKING_ID, 0, MTP_MAX_ID, 0, 0);
	input_set_abs_params(ts_dev, ABS_MT_POSITION_X, 0, MTP_MAX_X, 0, 0);
	input_set_abs_params(ts_dev, ABS_MT_POSITION_Y, 0, MTP_MAX_Y, 0, 0);	

	ts_dev->name = MTP_NAME; /* android会根据它找到配置文件 */

	/* 注册 */
	input_register_device(ts_dev);

	/* 硬件相关操作 */
	INIT_WORK(&mtp_work, mtp_work_func);
	
	request_irq(MTP_IRQ, mtp_interrupt,
			IRQ_TYPE_EDGE_FALLING /*IRQF_TRIGGER_FALLING*/, "100ask_mtp", ts_dev);
	
	return 0;
}

static int __devexit mtp_remove(struct i2c_client *client)
{
	printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

	free_irq(MTP_IRQ, ts_dev);
	cancel_work_sync(&mtp_work);

	input_unregister_device(ts_dev);
	input_free_device(ts_dev);
	
	return 0;
}

static const struct i2c_device_id mtp_id_table[] = {
	{ "100ask_mtp", 0 },  /* 支持我们自己的mtp_driver自行探测到的I2C设备 */
	{ "ft5x0x_ts", 0},    /* 支持mach-tiny4412.c中注册的名为"ft5x0x_ts"的I2C设备 */
	{}
};

static int mtp_ft5x06_valid(struct i2c_client *client)
{
	u8 buf[32] = { 0 };
	int ret;
	
	printk("mtp_ft5x06_valid : addr = 0x%x\n", client->addr);

	/* 进一步判断设备的合法性 */
	buf[0] = 0xa3; /* chip vendor id */	
	ret = mtp_ft5x0x_i2c_rxdata(client, buf, 1);

	if (ret < 0) {
		printk("There is not real device, i2c read err\n");
		return ret;
	}
	printk("chip vendor id = 0x%x\n", buf[0]);
	
	if (buf[0] != 0x55){
		printk("There is not real device, val err\n");
		return -1;
	}
	return 0;
}

static int mtp_detect(struct i2c_client *client,
		       struct i2c_board_info *info)
{
	/* 能运行到这里, 表示该addr的设备是存在的
	 * 但是有些设备单凭地址无法分辨(A芯片的地址是0x50, B芯片的地址也是0x50)
	 * 还需要进一步读写I2C设备来分辨是哪款芯片
	 * detect就是用来进一步分辨这个芯片是哪一款，并且设置info->type
	 */
	printk("mtp_detect : addr = 0x%x\n", client->addr);

	if (mtp_ft5x06_valid(client) < 0)
		return -1;
	
	strlcpy(info->type, "100ask_mtp", I2C_NAME_SIZE);
	return 0;

	/* 返回0之后, 会创建一个新的I2C设备
	 * i2c_new_device(adapter, &info), 其中的info->type = "100ask_mtp"
	 */
}

static const unsigned short addr_list[] = { MTP_ADDR, I2C_CLIENT_END };

/* 1. 分配/设置i2c_driver */
static struct i2c_driver mtp_driver = {
	.class  = I2C_CLASS_HWMON, /* 表示去哪些适配器上找设备 */
	.driver	= {
		.name	= "100ask",
		.owner	= THIS_MODULE,
	},
	.probe		= mtp_probe,
	.remove		= __devexit_p(mtp_remove),
	.id_table	= mtp_id_table,
	.detect     = mtp_detect,  /* 用这个函数来检测设备确实存在 */
	.address_list	= addr_list,   /* 这些设备的地址 */
};

static int mtp_drv_init(void)
{
	/* 2. 注册i2c_driver */
	i2c_add_driver(&mtp_driver);
	
	return 0;
}

static void mtp_drv_exit(void)
{
	i2c_del_driver(&mtp_driver);
}


module_init(mtp_drv_init);
module_exit(mtp_drv_exit);
MODULE_LICENSE("GPL");


