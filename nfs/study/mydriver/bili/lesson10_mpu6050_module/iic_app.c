#include <stdio.h>
#include <linux/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

int main()
{
    int fd,ret;
    struct i2c_rdwr_ioctl_data e2prom_data;
	int reg=0x75;
    fd=open("/dev/i2c-0",O_RDWR);//打开eeprom设备文件结点
    if(fd<0)
    {
        perror("open error");
    }

    e2prom_data.nmsgs=2;
    e2prom_data.msgs=(struct i2c_msg*)malloc(e2prom_data.nmsgs*sizeof(struct i2c_msg));//分配空间
    if(!e2prom_data.msgs)
    {
        perror("malloc error");
        exit(1);
    }
    ioctl(fd,I2C_TIMEOUT,1);/*超时时间*/
    ioctl(fd,I2C_RETRIES,2);/*重复次数*/

    /*写eeprom*/
    e2prom_data.nmsgs=1;//由前面eeprom读写分析可知，写eeprom需要一条消息
    (e2prom_data.msgs[0]).len=2; //此消息的长度为2个字节，第一个字节是要写入数据的地址，第二个字节是要写入的数据
    (e2prom_data.msgs[0]).addr=0x69;//e2prom 设备地址
    (e2prom_data.msgs[0]).flags=0; //写
    (e2prom_data.msgs[0]).buf=(unsigned char*)malloc(2);
    (e2prom_data.msgs[0]).buf[0]=0x70;// e2prom 写入目标的地址
    (e2prom_data.msgs[0]).buf[1]=0x58;//写入的数据
   // ret=ioctl(fd,I2C_RDWR,(unsigned long)&e2prom_data);//通过ioctl进行实际写入操作，后面会详细分析
   // if(ret<0)
    //{
     //   perror("ioctl error1");
    //}
    //sleep(1);

    /*读eeprom*/
    e2prom_data.nmsgs=2;//读eeprom需要两条消息
    (e2prom_data.msgs[0]).len=1; //第一条消息实际是写eeprom，需要告诉eeprom需要读数据的地址，因此长度为1个字节
    (e2prom_data.msgs[0]).addr=0x69; // e2prom 设备地址
    (e2prom_data.msgs[0]).flags=0;//先是写
    (e2prom_data.msgs[0]).buf[0]=0x75;//e2prom上需要读的数据的地址
    (e2prom_data.msgs[1]).len=1;//第二条消息才是读eeprom，
    (e2prom_data.msgs[1]).addr=0x69;// e2prom 设备地址
    (e2prom_data.msgs[1]).flags=I2C_M_RD;//然后是读
    (e2prom_data.msgs[1]).buf=(unsigned char*)malloc(1);//存放返回值的地址。
    (e2prom_data.msgs[1]).buf[0]=0;//初始化读缓冲，读到的数据放到此缓冲区
    ret=ioctl(fd,I2C_RDWR,(unsigned long)&e2prom_data);//通过ioctl进行实际的读操作
    if(ret<0)
    {
        perror("ioctl error2");
    }

    printf("buff[0]=%x\n",(e2prom_data.msgs[1]).buf[0]);
    /***打印读出的值，没错的话，就应该是前面写的0x58了***/
    close(fd);

    return 0;
}
