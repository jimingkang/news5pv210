//#include "lib.h"
#include "int.h"

#define I2CCON0	(*(volatile unsigned int *)R_I2CCON0)
#define I2CSTAT0 (*(volatile unsigned int *)R_I2CSTAT0)
#define I2CADD0 (*(volatile unsigned int *)R_I2CADD0)
#define I2CCDS0 (*(volatile unsigned int *)R_I2CDS0)
#define GPD1CON (*(volatile unsigned int *)R_GPD1CON)
int wr=0;
void enable_global_IRQ();



void i2c0_int_func()
{
 
    static int step=0;
    
    if(I2CSTAT0 & 0x0f)       //是否由于错误产生的中断
    {
        I2CSTAT0 = 0xd0;
        step=0;
//        mputs("i2c error\r\n");
    }
 
    else if(wr==0)            //写EEPROM
    {
        switch(step)
        {
            case 0:    I2CCDS0 = 0;    //发送Word Address
                       step++;
                       break;
            case 1:    I2CCDS0 = 'a';    //发送字符‘a’
                       step++;
                       break;
            case 2:    I2CCDS0 = 'z';  //发送字符‘z’
                       step++;
                       break;
            case 3:    I2CSTAT0 = 0xd0; //发送停止位
  //                     mputs("----------------------");
                       step=0;
                       break;
        }
    }
 
    else if(wr==1)   //读EEPROM
    {
        switch(step)
        {
            case 0:    I2CCDS0 = 0;//发送Word Address
                       step++;
                       break;
            case 1:    I2CCDS0= 0xa0;  //转为主/收模式
                       I2CSTAT0 = 0xb0;
                       step++;
                       break;
            case 2:    I2CCDS0 = 0;   //发送Word Address
                       step++;
                       break;
    //        case 3:    mputc((char)I2CCDS0);  //读数据
    //                   step++;
     //                  break;
     //       case 4:    mputc((char)I2CCDS0);  //读数据
     //                  step++;
     //                  break;
            case 5:    I2CSTAT0 = 0x90;    //发送停止位
                       step=0;
                       break;
        }
    }
 
    I2CCON0 &= (~0x10);  //清除中断标志
  intc_clearvectaddr();  
//    clear_vicaddress();     //清除全部VICADDRESS
    enable_global_IRQ(); //打开全局IRQ中断
    
}
 
 
void i2c0_init()
{
    GPD1CON = GPD1CON & (~0xff) | 0x22;  //设置引脚为I2C0的SLC和SDA
    I2CCON0 = 0xef; //设置I2C总线时钟分频系数、使能中断、清除中断标志
    I2CSTAT0 |= 0x10;  //使能I2C数据输出
}

