/*
 * driver/input/touchscreen/ft5x06_touch.c
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/time.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/input.h>
#include <linux/device.h>
#include <linux/earlysuspend.h>
#include <linux/hrtimer.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <mach/gpio.h>
#include <plat/gpio-cfg.h>
#include <linux/irq.h>

#define FT5X06_I2C_BUS		(1)
#define FT5X06_I2C_ADDRESS	(0x38)

#define SCREEN_MAX_WIDTH	(800)
#define SCREEN_MAX_HEIGHT	(480)

#define TOUCH_MAX_WIDTH	    (800)
#define TOUCH_MAX_HEIGHT    (480)

#define K_BACK				(KEY_BACK & KEY_MAX)
#define K_MENU				(KEY_MENU & KEY_MAX)
#define K_HOME				(KEY_HOME & KEY_MAX)
#define K_SEARCH			(KEY_SEARCH & KEY_MAX)

static void ft5x06_power_on(void)
{
	/*
	 * reset pin of cap touchscreen
	 */
	s3c_gpio_setpull(S5PV210_GPH0(6), S3C_GPIO_PULL_UP);
	s3c_gpio_cfgpin(S5PV210_GPH0(6), S3C_GPIO_SFN(1));
	gpio_set_value(S5PV210_GPH0(6), 0);

	/* wait a moment */
	mdelay(10);
	gpio_set_value(S5PV210_GPH0(6), 1);

	/* wait ... */
	mdelay(10);
}

struct point_node
{
	unsigned int x;
	unsigned int y;
};

struct ft5x06_ts {
	struct workqueue_struct * wq;
	struct work_struct work;
	struct i2c_client * client;
	struct input_dev * input_dev;

	struct point_node node[5];
};

enum {
	FT5X0X_REG_THGROUP					= 0x80,
	FT5X0X_REG_THPEAK					= 0x81,
	FT5X0X_REG_THCAL					= 0x82,
	FT5X0X_REG_THWATER					= 0x83,
	FT5X0X_REG_THTEMP					= 0x84,
	FT5X0X_REG_THDIFF					= 0x85,
	FT5X0X_REG_CTRL						= 0x86,
	FT5X0X_REG_TIMEENTERMONITOR			= 0x87,
	FT5X0X_REG_PERIODACTIVE				= 0x88,
	FT5X0X_REG_PERIODMONITOR			= 0x89,
	FT5X0X_REG_HEIGHT_B					= 0x8a,
	FT5X0X_REG_MAX_FRAME				= 0x8b,
	FT5X0X_REG_DIST_MOVE				= 0x8c,
	FT5X0X_REG_DIST_POINT				= 0x8d,
	FT5X0X_REG_FEG_FRAME				= 0x8e,
	FT5X0X_REG_SINGLE_CLICK_OFFSET		= 0x8f,
	FT5X0X_REG_DOUBLE_CLICK_TIME_MIN	= 0x90,
	FT5X0X_REG_SINGLE_CLICK_TIME		= 0x91,
	FT5X0X_REG_LEFT_RIGHT_OFFSET		= 0x92,
	FT5X0X_REG_UP_DOWN_OFFSET			= 0x93,
	FT5X0X_REG_DISTANCE_LEFT_RIGHT		= 0x94,
	FT5X0X_REG_DISTANCE_UP_DOWN			= 0x95,
	FT5X0X_REG_ZOOM_DIS_SQR				= 0x96,
	FT5X0X_REG_RADIAN_VALUE				= 0x97,
	FT5X0X_REG_MAX_X_HIGH				= 0x98,
	FT5X0X_REG_MAX_X_LOW             	= 0x99,
	FT5X0X_REG_MAX_Y_HIGH            	= 0x9a,
	FT5X0X_REG_MAX_Y_LOW             	= 0x9b,
	FT5X0X_REG_K_X_HIGH            		= 0x9c,
	FT5X0X_REG_K_X_LOW             		= 0x9d,
	FT5X0X_REG_K_Y_HIGH            		= 0x9e,
	FT5X0X_REG_K_Y_LOW             		= 0x9f,
	FT5X0X_REG_AUTO_CLB_MODE			= 0xa0,
	FT5X0X_REG_LIB_VERSION_H 			= 0xa1,
	FT5X0X_REG_LIB_VERSION_L 			= 0xa2,
	FT5X0X_REG_CIPHER					= 0xa3,
	FT5X0X_REG_MODE						= 0xa4,
	FT5X0X_REG_PMODE					= 0xa5,
	FT5X0X_REG_FIRMID					= 0xa6,
	FT5X0X_REG_STATE					= 0xa7,
	FT5X0X_REG_FT5201ID					= 0xa8,
	FT5X0X_REG_ERR						= 0xa9,
	FT5X0X_REG_CLB						= 0xaa,
};

typedef enum
{
    ERR_OK,
    ERR_MODE,
    ERR_READID,
    ERR_ERASE,
    ERR_STATUS,
    ERR_ECC,
    ERR_DL_ERASE_FAIL,
    ERR_DL_PROGRAM_FAIL,
    ERR_DL_VERIFY_FAIL
} E_UPGRADE_ERR_TYPE;

typedef unsigned char					FTS_BYTE;
typedef unsigned short					FTS_WORD;
typedef unsigned int					FTS_DWRD;
typedef unsigned char					FTS_BOOL;

#define FTS_NULL						0x0
#define FTS_TRUE						0x1
#define FTS_FALSE						0x0

#define FTS_PACKET_LENGTH				128

static unsigned char CTPM_FW[]=
{
#include "x210ii_app.h"
};

static void delay_qt_ms(unsigned long  w_ms)
{
    unsigned long i;
    unsigned long j;

    for (i = 0; i < w_ms; i++)
    {
        for (j = 0; j < 1000; j++)
        {
            udelay(1);
        }
    }
}

static int ft5x06_read_reg(struct i2c_client * client, uint8_t addr, uint8_t * data)
{
    uint8_t buf[2];
    struct i2c_msg msgs[2];
    int ret;

    buf[0] = addr;

    msgs[0].addr = client->addr;
    msgs[0].flags = 0;
    msgs[0].len = 1;
    msgs[0].buf = buf;

    msgs[1].addr = client->addr;
    msgs[1].flags = I2C_M_RD;
    msgs[1].len = 1;
    msgs[1].buf = buf;

    ret = i2c_transfer(client->adapter, msgs, 2);
    if(ret < 0)
        printk("msg i2c read error\n");

    *data = buf[0];
    return ret;
}

static int ft5x06_i2c_txdata(struct i2c_client * client, char * txdata, int length)
{
    int ret;

    struct i2c_msg msg[] = {
        {
            .addr	= client->addr,
            .flags	= 0,
            .len	= length,
            .buf	= txdata,
        },
    };

    ret = i2c_transfer(client->adapter, msg, 1);
    if (ret < 0)
        printk("%s i2c write error: %d\n", __func__, ret);

    return ret;
}

static int ft5x06_write_reg(struct i2c_client * client, uint8_t addr, uint8_t data)
{
	uint8_t buf[3];
    int ret = -1;

    buf[0] = addr;
    buf[1] = data;

    ret = ft5x06_i2c_txdata(client, buf, 2);
    if (ret < 0)
    {
        printk("write reg failed! %#x ret: %d", buf[0], ret);
        return -1;
    }

    return 0;
}

static FTS_BOOL i2c_read_interface(struct i2c_client * client, FTS_BYTE bt_ctpm_addr, FTS_BYTE* pbt_buf, FTS_DWRD dw_lenth)
{
    int ret;

    ret = i2c_master_recv(client, pbt_buf, dw_lenth);

    if(ret<=0)
    {
        printk("[FTS]i2c_read_interface error\n");
        return FTS_FALSE;
    }

    return FTS_TRUE;
}

static FTS_BOOL i2c_write_interface(struct i2c_client * client, FTS_BYTE bt_ctpm_addr, FTS_BYTE* pbt_buf, FTS_DWRD dw_lenth)
{
    int ret;

    ret = i2c_master_send(client, pbt_buf, dw_lenth);
    if(ret<=0)
    {
        printk("[FTS]i2c_write_interface error line = %d, ret = %d\n", __LINE__, ret);
        return FTS_FALSE;
    }

    return FTS_TRUE;
}

static FTS_BOOL cmd_write(struct i2c_client * client, FTS_BYTE btcmd,FTS_BYTE btPara1,FTS_BYTE btPara2,FTS_BYTE btPara3,FTS_BYTE num)
{
    FTS_BYTE write_cmd[4] = {0};

    write_cmd[0] = btcmd;
    write_cmd[1] = btPara1;
    write_cmd[2] = btPara2;
    write_cmd[3] = btPara3;
    return i2c_write_interface(client, FT5X06_I2C_ADDRESS, write_cmd, num);
}

static FTS_BOOL byte_write(struct i2c_client * client, FTS_BYTE* pbt_buf, FTS_DWRD dw_len)
{
    return i2c_write_interface(client, FT5X06_I2C_ADDRESS, pbt_buf, dw_len);
}

static FTS_BOOL byte_read(struct i2c_client * client, FTS_BYTE* pbt_buf, FTS_BYTE bt_len)
{
    return i2c_read_interface(client, FT5X06_I2C_ADDRESS, pbt_buf, bt_len);
}

static uint8_t ft5x06_read_fw_ver(struct i2c_client * client)
{
	uint8_t ver;

	ft5x06_read_reg(client, FT5X0X_REG_FIRMID, &ver);
	return (ver);
}

static uint8_t fts_ctpm_get_i_file_ver(void)
{
    unsigned int ui_sz;

    ui_sz = sizeof(CTPM_FW);

    if (ui_sz > 2)
        return CTPM_FW[ui_sz - 2];
    else
        return 0xff;
}

E_UPGRADE_ERR_TYPE fts_ctpm_fw_upgrade(struct i2c_client * client, FTS_BYTE * pbt_buf, FTS_DWRD dw_lenth)
{
    FTS_BYTE reg_val[2] = {0};
    FTS_DWRD i = 0;

    FTS_DWRD  packet_number;
    FTS_DWRD  j;
    FTS_DWRD  temp;
    FTS_DWRD  lenght;
    FTS_BYTE  packet_buf[FTS_PACKET_LENGTH + 6];
    FTS_BYTE  auc_i2c_write_buf[10];
    FTS_BYTE bt_ecc;
    int      i_ret;

    /*********Step 1:Reset  CTPM *****/
    /*write 0xaa to register 0xfc*/
    ft5x06_write_reg(client, 0xfc,0xaa);
    delay_qt_ms(50);
     /*write 0x55 to register 0xfc*/
    ft5x06_write_reg(client, 0xfc,0x55);
    printk("[FTS] Step 1: Reset CTPM test\n");

    delay_qt_ms(30);


    /*********Step 2:Enter upgrade mode *****/
    auc_i2c_write_buf[0] = 0x55;
    auc_i2c_write_buf[1] = 0xaa;
    do
    {
        i ++;
        i_ret = ft5x06_i2c_txdata(client, auc_i2c_write_buf, 2);
        delay_qt_ms(5);
    }while(i_ret <= 0 && i < 5 );

    /*********Step 3:check READ-ID***********************/
    cmd_write(client,0x90,0x00,0x00,0x00,4);
    byte_read(client,reg_val,2);
    if (reg_val[0] == 0x79 && reg_val[1] == 0x3)
    {
        printk("[FTS] Step 3: CTPM ID,ID1 = 0x%x,ID2 = 0x%x\n",reg_val[0],reg_val[1]);
    }
    else
    {
        return ERR_READID;
        //i_is_new_protocol = 1;
    }

    cmd_write(client,0xcd,0x0,0x00,0x00,1);
    byte_read(client,reg_val,1);
    printk("[FTS] bootloader version = 0x%x\n", reg_val[0]);

     /*********Step 4:erase app and panel paramenter area ********************/
    cmd_write(client,0x61,0x00,0x00,0x00,1);  //erase app area
    delay_qt_ms(1500);
    cmd_write(client,0x63,0x00,0x00,0x00,1);  //erase panel parameter area
    delay_qt_ms(100);
    printk("[FTS] Step 4: erase. \n");

    /*********Step 5:write firmware(FW) to ctpm flash*********/
    bt_ecc = 0;
    printk("[FTS] Step 5: start upgrade. \n");
    dw_lenth = dw_lenth - 8;
    packet_number = (dw_lenth) / FTS_PACKET_LENGTH;
    packet_buf[0] = 0xbf;
    packet_buf[1] = 0x00;
    for (j=0;j<packet_number;j++)
    {
        temp = j * FTS_PACKET_LENGTH;
        packet_buf[2] = (FTS_BYTE)(temp>>8);
        packet_buf[3] = (FTS_BYTE)temp;
        lenght = FTS_PACKET_LENGTH;
        packet_buf[4] = (FTS_BYTE)(lenght>>8);
        packet_buf[5] = (FTS_BYTE)lenght;

        for (i=0;i<FTS_PACKET_LENGTH;i++)
        {
            packet_buf[6+i] = pbt_buf[j*FTS_PACKET_LENGTH + i];
            bt_ecc ^= packet_buf[6+i];
        }

        byte_write(client,&packet_buf[0],FTS_PACKET_LENGTH + 6);
        delay_qt_ms(FTS_PACKET_LENGTH/6 + 1);
        if ((j * FTS_PACKET_LENGTH % 1024) == 0)
        {
              printk("[FTS] upgrade the 0x%x th byte.\n", ((unsigned int)j) * FTS_PACKET_LENGTH);
        }
    }

    if ((dw_lenth) % FTS_PACKET_LENGTH > 0)
    {
        temp = packet_number * FTS_PACKET_LENGTH;
        packet_buf[2] = (FTS_BYTE)(temp>>8);
        packet_buf[3] = (FTS_BYTE)temp;

        temp = (dw_lenth) % FTS_PACKET_LENGTH;
        packet_buf[4] = (FTS_BYTE)(temp>>8);
        packet_buf[5] = (FTS_BYTE)temp;

        for (i=0;i<temp;i++)
        {
            packet_buf[6+i] = pbt_buf[ packet_number*FTS_PACKET_LENGTH + i];
            bt_ecc ^= packet_buf[6+i];
        }

        byte_write(client,&packet_buf[0],temp+6);
        delay_qt_ms(20);
    }

    //send the last six byte
    for (i = 0; i<6; i++)
    {
        temp = 0x6ffa + i;
        packet_buf[2] = (FTS_BYTE)(temp>>8);
        packet_buf[3] = (FTS_BYTE)temp;
        temp =1;
        packet_buf[4] = (FTS_BYTE)(temp>>8);
        packet_buf[5] = (FTS_BYTE)temp;
        packet_buf[6] = pbt_buf[ dw_lenth + i];
        bt_ecc ^= packet_buf[6];

        byte_write(client,&packet_buf[0],7);
        delay_qt_ms(20);
    }


    /*********Step 6: read out checksum***********************/
    /*send the opration head*/
    cmd_write(client,0xcc,0x00,0x00,0x00,1);
    byte_read(client,reg_val,1);
    printk("[FTS] Step 6:  ecc read 0x%x, new firmware 0x%x. \n", reg_val[0], bt_ecc);
    if(reg_val[0] != bt_ecc)
    {
        return ERR_ECC;
    }

    /*********Step 7: reset the new FW***********************/
    cmd_write(client,0x07,0x00,0x00,0x00,1);

    msleep(300);  //make sure CTP startup normally

    return ERR_OK;
}

int fts_ctpm_auto_clb(struct i2c_client * client)
{
    unsigned char uc_temp;
    unsigned char i ;

    printk("[FTS] start auto CLB.\n");
    msleep(200);

    ft5x06_write_reg(client, 0, 0x40);
    delay_qt_ms(100);   //make sure already enter factory mode
    ft5x06_write_reg(client, 2, 0x4);  //write command to start calibration
    delay_qt_ms(300);

    for(i=0;i<100;i++)
    {
        ft5x06_read_reg(client, 0, &uc_temp);
        if ( ((uc_temp&0x70)>>4) == 0x0)  //return to normal mode, calibration finish
        {
            break;
        }
        delay_qt_ms(200);
        printk("[FTS] waiting calibration %d\n",i);
    }

    printk("[FTS] calibration OK.\n");

    msleep(300);
    ft5x06_write_reg(client, 0, 0x40);  //goto factory mode
    delay_qt_ms(100);   //make sure already enter factory mode
    ft5x06_write_reg(client, 2, 0x5);  //store CLB result
    delay_qt_ms(300);
    ft5x06_write_reg(client, 0, 0x0); //return to normal mode
    msleep(300);

    printk("[FTS] store CLB result OK.\n");
    return 0;
}

static int fts_ctpm_fw_upgrade_with_i_file(struct i2c_client * client)
{
   FTS_BYTE * pbt_buf = FTS_NULL;
   int i_ret;

   pbt_buf = CTPM_FW;

   /*
    * call the upgrade function
    */
   i_ret = fts_ctpm_fw_upgrade(client, pbt_buf, sizeof(CTPM_FW));
   if(i_ret != 0)
   {
       printk("[FTS] upgrade failed i_ret = %d.\n", i_ret);
   }
   else
   {
       printk("[FTS] upgrade successfully.\n");
       fts_ctpm_auto_clb(client);
   }

   return i_ret;
}

static int fts_ctpm_auto_upg(struct i2c_client * client)
{
	unsigned char uc_host_fm_ver;
	unsigned char uc_tp_fm_ver;
	int i_ret;

	uc_tp_fm_ver = ft5x06_read_fw_ver(client);
	uc_host_fm_ver = fts_ctpm_get_i_file_ver();
	if (uc_tp_fm_ver == 0xa6 || uc_tp_fm_ver < uc_host_fm_ver)
	{
		msleep(100);
		printk("[FTS] uc_tp_fm_ver = 0x%x, uc_host_fm_ver = 0x%x\n", uc_tp_fm_ver, uc_host_fm_ver);

		i_ret = fts_ctpm_fw_upgrade_with_i_file(client);
		if (i_ret == 0)
		{
			msleep(300);
			uc_host_fm_ver = ft5x06_read_fw_ver(client);
			printk("[FTS] upgrade to new version 0x%x\n", uc_host_fm_ver);
		}
		else
		{
			printk("[FTS] upgrade failed ret=%d.\n", i_ret);
		}
	}

	return 0;
}

static int ft5x06_read_coordinates(struct i2c_client * client, uint8_t * buf, uint8_t len)
{
	struct i2c_msg msgs[2];
    uint8_t msgbuf[1] = { 0x0 };
    int ret;

	msgs[0].flags = !I2C_M_RD;
	msgs[0].addr = client->addr;
	msgs[0].len = 1;
	msgs[0].buf = &msgbuf[0];

	msgs[1].flags = I2C_M_RD;
	msgs[1].addr = client->addr;
	msgs[1].len = len;
	msgs[1].buf = &buf[0];

	ret = i2c_transfer(client->adapter, msgs, 2);
    return ret;
}

static void ft5x06_ts_work_func(struct work_struct * work)
{
	struct ft5x06_ts * ts = container_of(work, struct ft5x06_ts, work);
	uint8_t buf[32] = { 0 };
	unsigned int X, Y;
	unsigned int x, y, event, id;
	int i;

	ft5x06_read_coordinates(ts->client, buf, 32);

	for(i = 0; i < 5; i++)
	{
		X = (buf[i*6 + 3])<<8 | buf[i*6 + 4];
		Y = (buf[i*6 + 5])<<8 | buf[i*6 + 6];

		x = X & 0xfff;
		y = Y & 0xfff;
		y = SCREEN_MAX_HEIGHT - y;
	
		event = (X >> 14) & 0x3;
		id = (Y >> 12) & 0xf;

		if(id >= 0 && id <= 4)
		{
			ts->node[id].x = x;
			ts->node[id].y = y;

			if((event == 0) || (event == 0x02))
			{
				input_report_abs(ts->input_dev, ABS_MT_TOUCH_MAJOR, 255);
				input_report_abs(ts->input_dev, ABS_MT_POSITION_X, ts->node[id].x);
				input_report_abs(ts->input_dev, ABS_MT_POSITION_Y, ts->node[id].y);
				input_report_abs(ts->input_dev, ABS_PRESSURE, 255);
				input_report_key(ts->input_dev, BTN_TOUCH, 1);
				input_report_abs(ts->input_dev, ABS_MT_TRACKING_ID, id);
				input_mt_sync(ts->input_dev);

				// printk("[%d]down: x = %4d, y = %4d\n", id, ts->node[id].x, ts->node[id].y);
			}
			else if(event == 0x01)
			{
				input_report_abs(ts->input_dev, ABS_MT_TOUCH_MAJOR, 0);
				input_report_abs(ts->input_dev, ABS_MT_POSITION_X, ts->node[id].x);
				input_report_abs(ts->input_dev, ABS_MT_POSITION_Y, ts->node[id].y);
				input_report_abs(ts->input_dev, ABS_PRESSURE, 0);
				input_report_key(ts->input_dev, BTN_TOUCH, 0);
				input_report_abs(ts->input_dev, ABS_MT_TRACKING_ID, id);
				input_mt_sync(ts->input_dev);

				// printk("[%d]up: x = %4d, y = %4d\n", id, ts->node[id].x, ts->node[id].y);
			}
		}
	}

	input_sync(ts->input_dev);
	enable_irq(ts->client->irq);
}

static irqreturn_t ft5x06_ts_irq_handler(int irq, void * dev_id)
{
	struct ft5x06_ts * ts = dev_id;

	disable_irq_nosync(ts->client->irq);
	queue_work(ts->wq, &ts->work);

	return IRQ_HANDLED;
}

static int ft5x06_iic_probe(struct i2c_client * client, const struct i2c_device_id * id)
{
	struct ft5x06_ts * ts;
	int ret;
	unsigned char uc_reg_value;

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_SMBUS_WORD_DATA))
		return -ENODEV;

	ts = kzalloc(sizeof(*ts), GFP_KERNEL);
	if(!ts)
		return -ENOMEM;

	ts->wq = create_workqueue("ft5x06_wq");
	if(!ts->wq)
	{
		kfree(ts);
		return -ENOMEM;
	}

	INIT_WORK(&ts->work, ft5x06_ts_work_func);
	ts->client = client;
	i2c_set_clientdata(client, ts);

	ts->input_dev = input_allocate_device();
	if (ts->input_dev == NULL)
	{
		destroy_workqueue(ts->wq);
		kfree(ts);
		return -ENOMEM;
	}

	ts->input_dev->name = "ft5x06-ts";
	ts->input_dev->phys = "input/ts";
	ts->input_dev->id.bustype = BUS_I2C;
	ts->input_dev->id.vendor = 0xDEAD;
	ts->input_dev->id.product = 0xBEEF;
	ts->input_dev->id.version = 10427;

    set_bit(EV_SYN, ts->input_dev->evbit);
    set_bit(EV_KEY, ts->input_dev->evbit);
    set_bit(EV_ABS, ts->input_dev->evbit);
    set_bit(BTN_TOUCH, ts->input_dev->keybit);
	set_bit(BTN_2, ts->input_dev->keybit);

    set_bit(K_BACK, ts->input_dev->keybit);
    set_bit(K_MENU, ts->input_dev->keybit);
    set_bit(K_HOME, ts->input_dev->keybit);
    set_bit(K_SEARCH, ts->input_dev->keybit);

    input_set_abs_params(ts->input_dev, ABS_MT_WIDTH_MAJOR, 0, 255, 0, 0);
    input_set_abs_params(ts->input_dev, ABS_MT_POSITION_X, 0, SCREEN_MAX_WIDTH, 0, 0);
    input_set_abs_params(ts->input_dev, ABS_MT_POSITION_Y, 0, SCREEN_MAX_HEIGHT, 0, 0);
    input_set_abs_params(ts->input_dev, ABS_PRESSURE, 0, 255, 0, 0);
    input_set_abs_params(ts->input_dev, ABS_MT_TRACKING_ID, 0, 5, 0, 0);

    ret = input_register_device(ts->input_dev);
	if(ret)
	{
		destroy_workqueue(ts->wq);
		kfree(ts);
		return ret;
	}	

	client->irq = IRQ_EINT7;

	/* config interrupt pin */
	s3c_gpio_cfgpin(S5PV210_GPH0(7), S3C_GPIO_SFN(0xf));
	s3c_gpio_setpull(S5PV210_GPH0(7), S3C_GPIO_PULL_NONE);

	irq_set_irq_type(client->irq, IRQ_TYPE_EDGE_FALLING);
	ret = request_irq(client->irq, ft5x06_ts_irq_handler, IRQ_TYPE_EDGE_FALLING, client->name, ts);
	if(ret)
	{
		destroy_workqueue(ts->wq);
		kfree(ts);
		return ret;
	}

	disable_irq_nosync(IRQ_EINT7);

	msleep(150);

	uc_reg_value = ft5x06_read_fw_ver(client);
	printk("[FTS] Firmware version = 0x%x\n", uc_reg_value);

    ft5x06_read_reg(client, FT5X0X_REG_PERIODACTIVE, &uc_reg_value);
    printk("[FTS] report rate is %dHz.\n", uc_reg_value * 10);
    ft5x06_read_reg(client, FT5X0X_REG_THGROUP, &uc_reg_value);
    printk("[FTS] touch threshold is %d.\n", uc_reg_value * 4);

	fts_ctpm_auto_upg(client);

	enable_irq(IRQ_EINT7);
	return 0;
}

static int ft5x06_iic_remove(struct i2c_client * client)
{
	struct ft5x06_ts * ts = i2c_get_clientdata(client);

	free_irq(client->irq, ts);
	i2c_set_clientdata(client, NULL);
	input_unregister_device(ts->input_dev);
	if(ts->input_dev)
		kfree(ts->input_dev);
	kfree(ts);

	return 0;
}

static int ft5x06_iic_suspend(struct i2c_client * client, pm_message_t message)
{
	return 0;
}

static int ft5x06_iic_resume(struct i2c_client * client)
{
	ft5x06_power_on();

	return 0;
}

static const struct i2c_device_id ft5x06_iic_id[] = {
	{ "ft5x06-iic", 0},
	{ }
};

static struct i2c_driver ft5x06_iic_driver = {
	.driver		= {
		.name	= "ft5x06-iic",
	},

	.probe		= ft5x06_iic_probe,
	.remove		= ft5x06_iic_remove,
	.suspend	= ft5x06_iic_suspend,
	.resume		= ft5x06_iic_resume,
	.id_table	= ft5x06_iic_id,
};

static int ft5x06_ts_probe(struct platform_device * pdev)
{
	struct i2c_adapter * adapter;
	struct i2c_client * client;
	struct i2c_board_info info;
	int ret;

	adapter = i2c_get_adapter(FT5X06_I2C_BUS);
	if(adapter == NULL)
		return -ENODEV;

	memset(&info, 0, sizeof(struct i2c_board_info));
	info.addr = FT5X06_I2C_ADDRESS;
	strlcpy(info.type, "ft5x06-iic", I2C_NAME_SIZE);

	client = i2c_new_device(adapter, &info);
    if(!client)
    {
       	printk("Unable to add I2C device for 0x%x\n", info.addr);
       	return -ENODEV;
    }
    i2c_put_adapter(adapter);

	ret = i2c_add_driver(&ft5x06_iic_driver);
	if(ret)
		return ret;

	return 0;
}

static int ft5x06_ts_remove(struct platform_device * pdev)
{
	i2c_del_driver(&ft5x06_iic_driver);
	return 0;
}

static int ft5x06_ts_suspend(struct platform_device * dev, pm_message_t state)
{
	return 0;
}

static int ft5x06_ts_resume(struct platform_device * dev)
{
	return 0;
}

static struct platform_device ft5x06_ts_device = {
	.name	= "ft5x06-ts",
	.id		= -1,
};

static struct platform_driver ft5x06_ts_driver = {
	.driver		= {
		.name	= "ft5x06-ts",
	},
	.probe		= ft5x06_ts_probe,
	.remove		= ft5x06_ts_remove,
	.suspend	= ft5x06_ts_suspend,
	.resume		= ft5x06_ts_resume,
};

static int __init ft5x06_ts_init(void)
{
    int res;
	
	printk("Initial ft5x06 Touch Driver\n");

	ft5x06_power_on();

	res = platform_device_register(&ft5x06_ts_device);
	if(res)
	{
		printk("failed to register platform device\n");
	}

	res = platform_driver_register(&ft5x06_ts_driver);
	if(res != 0)
	{
		printk("fail to register platform driver\n");
		return res;
	}

    return 0;
}

void __exit ft5x06_ts_exit(void)
{
	platform_driver_unregister(&ft5x06_ts_driver);
}

module_init(ft5x06_ts_init);
module_exit(ft5x06_ts_exit);

MODULE_AUTHOR("jerryjianjun@gmail.com");
MODULE_DESCRIPTION("ft5x06 Touchscreen Driver");
MODULE_LICENSE("GPL");
