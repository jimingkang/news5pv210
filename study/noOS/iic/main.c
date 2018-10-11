#include "stdio.h"
#include "int.h"
#include "main.h"
#define I2CCON0	(*(volatile unsigned int *)R_I2CCON0)
#define I2CSTAT0 (*(volatile unsigned int *)R_I2CSTAT0)
#define I2CADD0 (*(volatile unsigned int *)R_I2CADD0)
#define I2CCDS0 (*(volatile unsigned int *)R_I2CDS0)


#define KEY_INTNUM		NUM_EINT9		// 暂时随便找的，下节课会具体改
#define KEY_EINT2		NUM_EINT2		// left
#define KEY_EINT3		NUM_EINT3		// down
#define KEY_EINT16_19	NUM_EINT16_31	// 其余4个共用的

void key_isr(void)
{
	// 暂时随便写的，下节课正式的会写到key.c中去
}

int main(void)
{	
    unsigned long test,temp;
int wr=0;	
uart_init();
key_init();
	
   // i2c0_init();
	// 如果程序中要使用中断，就要调用中断初始化来初步初始化中断控制器
	system_init_exception();
	
	// 绑定isr到中断控制器硬件
intc_setvectaddr(KEY_EINT2, isr_eint2);
	intc_setvectaddr(KEY_EINT3, isr_eint3);
	intc_setvectaddr(KEY_EINT16_19, isr_eint16171819);
	
	// 使能中断
	intc_enable(KEY_EINT2);
	intc_enable(KEY_EINT3);
	intc_enable(KEY_EINT16_19);
	       

//intc_setvectaddr(46,i2c0_int_func);
 //      intc_enable(46);

    
 
	
	return 0;
}
