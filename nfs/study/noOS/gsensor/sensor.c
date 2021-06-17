int  gsensor_write_byte(uint8_t add, uint8_t data)

{

    int timeout =TIMEOUT_VALUE;

    volatile int i;

    int res=0;

    //发设备地址

    I2CDS = SLAVE_ADDRESS_W;

    I2CSTAT = 0xf0;

#ifdef DETECT_TIMEOUT

    while( (I2CCON&0x10) == 0){ //发设备地址响应超时判断，超时返回1

       if(timeout-- == 0)

       {     

           res = 1;

           return res;

       }         

       delay_us(10);

    }

#else

    while( (I2CCON&0x10) == 0);

#endif

    if(I2CSTAT&0x01)    //如果设备地址（写）无响应，置为标志

    {

       res |=  (1<<7 | 1<<3);

    }

//  printf("dev(write) I2CSTAT: 0x%x\r\n",I2CSTAT);

    //发数据地址

    I2CDS = add;

    I2CCON &= ~(1<<4);  //清除中断，重新发起写操作,写地址

#ifdef DETECT_TIMEOUT

    timeout =TIMEOUT_VALUE;

    while( (I2CCON&0x10) == 0){ //发数据地址响应超时判断，超时返回2

       if(timeout-- == 0)

       {

           res = 2;

           return res;

       }

       delay_us(10);

    }

#else

    while( (I2CCON&0x10) == 0);

#endif

    if(I2CSTAT&0x01 )  //如果数据地址无响应，置为标志

    {

       res |=  (1<<7 | 1<< 5);

    }

//  printf("add I2CSTAT: 0x%x\r\n",I2CSTAT);

    //发送数据

    I2CDS = data;

    I2CCON &= ~(1<<4);  //清除中断，重新发起写操作，写数据

#ifdef DETECT_TIMEOUT

    timeout =TIMEOUT_VALUE;

    while( (I2CCON&0x10) == 0){ //发数据响应超时判断，超时返回3

       if(timeout-- == 0)

       {

           res = 3;

           return res;

       }

       delay_us(10);

    }

#else

    while( (I2CCON&0x10) == 0);

#endif

    if(I2CSTAT&0x01 )  //如果写数据无响应，置为标志

    {

       res |=  (1<<7 | 1<< 6);

    }

//  printf("data(write) I2CSTAT: 0x%x\r\n",I2CSTAT);

    //发停止位

    I2CSTAT = 0xd0;  

#ifdef DETECT_TIMEOUT

    timeout = TIMEOUT_VALUE;

    while( (I2CSTAT&0x20) == 0){ //判断IIC BUS不忙超时判断，超时返回4

       if(timeout-- == 0)

       {

           res = 4;

           return res;

       }

       delay_us(10);

    }

#else

    while( (I2CSTAT&0x20) == 0);

#endif

    I2CCON = IIC_CON_VALUE;  //重新设置I2CCON，为下一次做准备

    for(i=0; i<50;i++);     //延时,以保证两次IIC操作间隔不太快

    return res;

}
