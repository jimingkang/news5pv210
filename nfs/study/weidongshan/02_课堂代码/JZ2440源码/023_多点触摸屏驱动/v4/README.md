
Demo driver for multi touch panel, it is only a Framework, for weidongshan's android video tutorial.  
  
It uses a i2c Multi-Touch Panel Controller.  
  
v1: Demo driver for multi touch panel, it is only a Framework  
    
v2: There are some errors in v1  
  
v3:  FT5x06 driver for tiny4412  
  
v4:  add property in driver, android does not need .idc file again    
  
v3,v4 usage:  
a. remove old driver ft5x06_ts.c  
   modify drivers/input/touchscreen/Makefile:  
obj-$(CONFIG_TOUCHSCREEN_FT5X0X)                += ft5x06_ts.o  
change to:    
obj-$(CONFIG_TOUCHSCREEN_FT5X0X)                += mtp_input.o  
   
  
b. remove i2c device:  
   modify arch/arm/mach-exynos/mach-tiny4412.c  
   remove :  
   i2c_register_board_info(1, i2c_devs1, ARRAY_SIZE(i2c_devs1));  
  
or: modify mtp_input.c:   
   static const struct i2c_device_id mtp_id_table[] = {  
	{ "100ask_mtp", 0 },  
	{ "ft5x0x_ts", 0},  // add this line, we have added it in v3     
	{}  
};    
  
c. make zImage, and use new zImage   







