#include "stdio.h"
#include "main.h"
#include "int.h"
void uart_init(void);


#define KEY_EINT2		NUM_EINT2		// left
#define KEY_EINT3		NUM_EINT3		// down
#define KEY_EINT16_19	NUM_EINT16_31	// 其余4个共用的
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


int main(void)
{
        

	uart_init();
printf("main \r\n");	
      
	
//lcd_test();	
	
/*char a = 'T';
	putc('a');
	putc('b');
	putc('c');
	while (1)
	{a=getc();
	printf("test for printf, a = %d.\r\n", a);
	}
*/
            
	key_init();
	key_polling();
	
/*
 
       key_init_interrupt();
	//	printf("delete  system_init_exception for 0xD0037418 \r\n");	
	printf("delete  system_init_exception for irq nop botom\r\n");	
	// 如果程序中要使用中断，就要调用中断初始化来初步初始化中断控制器
	//system_init_exception();
	//intc_init();

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
