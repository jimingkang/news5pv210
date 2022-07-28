#include "stdio.h"
#include "main.h"
#include "int.h"

#include "debug.h"
#include "nand.h"
void uart_init(void);


#define KEY_EINT2		NUM_EINT2		// left
#define KEY_EINT3		NUM_EINT3		// down
#define KEY_EINT16_19	NUM_EINT16_31	// 其余4个共用的

typedef void (*pBL2Type)(void);

void delay(int i)
{
	volatile int j = 100;
	while (i--)
		while(j--);
}
void mydelay()
{
int i=10000;
	volatile int j = 100;
	while (i--)
		while(j--);
}

void nand_operation_test()
{
    char buf[1024];
    int ret;
    
    erase_flash(0x700000);

    nand_write("123450",0x700000,10);

    ret = nand_read(buf,0x700000,8);
    if(ret < 0){
        return ;
    }
    buf[ret] = 0;
    printf("the nand read buf is %s\n",buf);
}

void Hsmmc_ReadBlock_test()
{
    char buf[512];
//char buf2[512];
char * buf2=(char *)0x38000000;
    int ret;
int i=0;
for(i=0;i<1024;i++){
buf[i]=i;
}
    
   // erase_flash(0x45);
//Hsmmc_EraseBlock(500,501);
  //  Hsmmc_WriteBlock(buf,500,1);

    ret = Hsmmc_ReadBlock(buf2,45,32);
    if(ret < 0){
        return ;
    }
   // buf[ret] = 0;
   // printf("the nand read buf is %s\n",buf2[8]);
pBL2Type p2 = (pBL2Type)buf2;
	p2();

}


int main(void)
{
        

	uart_init();
	printf("after uart init \r\n");	
//  
 //	nand_lowlevel_init();
   // 	printf("the armclock is %d\n",get_arm_clk());
    //	nand_init_chip();
//nand_operation_test();

 Hsmmc_Init();
Hsmmc_ReadBlock_test();
//
//
	//  key_init();
       // irq_init();

	
            
	//	key_init();
	//	key_polling();

	//lcd_test();	
	
	/**
       key_init_interrupt();
	//	printf("delete  system_init_exception for 0xD0037418 \r\n");	
	printf("delete  system_init_exception and test for sdcard relocate\r\n");	
	// 如果程序中要使用中断，就要调用中断初始化来初步初始化中断控制器
	//system_init_exception();
	intc_init();

	printf("-------------key interrypt test--------------");
	
	// 绑定isr到中断控制器硬件
	intc_setvectaddr(KEY_EINT2, isr_eint2);
	intc_setvectaddr(KEY_EINT3, isr_eint3);
	intc_setvectaddr(KEY_EINT16_19, isr_eint16171819);
	
	// 使能中断
	intc_enable(KEY_EINT2);
	intc_enable(KEY_EINT3);
	intc_enable(KEY_EINT16_19);
	// 在这里加个心跳
	//while (1)
	//{
	//	printf("B ");
	//	delay(100);
	//}

*/
	return 0;
}
