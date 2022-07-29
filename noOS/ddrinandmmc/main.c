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


void Hsmmc_ReadBlock_test()
{

//printf("after uart init \r\n");	
Hsmmc_Init();
   unsigned int * start=(char *)0x20000000;
    int ret;
    
   // erase_flash(0x45);
//Hsmmc_EraseBlock(500,501);
  //  Hsmmc_WriteBlock(buf,500,1);

    ret = Hsmmc_ReadBlock(start,49,32);
    if(ret < 0){
        return ;
    }
   //pBL2Type p2 = (pBL2Type)start;
//	p2();
//main();

}


int main(void)
{
        char a='a';

        putc('d');   
//while(1){
//a=getc();
//putc(a); 
//}
    
 
 printf("jump to main\n");	
  printf("addressof a=%x\n", &a);	



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
