#include "stdio.h"
#include "main.h"
#include "int.h"

#include "debug.h"



#define KEY_EINT2		NUM_EINT2		// left
#define KEY_EINT3		NUM_EINT3		// down
#define KEY_EINT16_19	NUM_EINT16_31	// ����4�����õ�

#define DDR_START_ADDR	0x20008000
/*
void delay(int i)
{
	volatile int j = 100;
	while (i--)
		while(j--);
}
*/
void mydelay()
{
	int i=10000;
	volatile int j = 100;
	while (i--)
		while(j--);
}

void Hsmmc_ReadBlock_test()
{
    char buf[1024];
    char buf2[1024];
    int ret;    
   // erase_flash(0x45);
   //Hsmmc_EraseBlock(500,501);
   // Hsmmc_WriteBlock(buf,500,1);


    ret = Hsmmc_ReadBlock((unsigned int *)DDR_START_ADDR,1,1);
    if(ret < 0){
        return ;
    }

}

void init_sdcard(void){
	
	printf("after uart init \r\n");	


       Hsmmc_Init();
       Hsmmc_ReadBlock_test();
       
    printf("the Hsmmc_ReadBlock is done DDR_START_ADDR:%d\n",(*(unsigned int *)DDR_START_ADDR));


}
/**/
int main(void)
{
clock_init();
uart_init();
buzzer_init();
buzzer_on();
//led3();
printf("main =\n");

//init_sdcard();
//printf("ge mem from =%x\n",(*(unsigned int *)DDR_START_ADDR));

   while (1){
    printf(" wating for getchar\n");

   	char c=getc();
   	printf("getchar=%d\n",c);
     }

      // key_init();
      // irq_init();            
	//key_init();
	//key_polling();

	//lcd_test();	
	
	/*
       key_init_interrupt();
	//	printf("delete  system_init_exception for 0xD0037418 \r\n");	
	printf("delete  system_init_exception and test for sdcard relocate\r\n");	
	// ���������Ҫʹ���жϣ���Ҫ�����жϳ�ʼ����������ʼ���жϿ�����
	//system_init_exception();
	intc_init();

	printf("-------------key interrypt test--------------");
	
	// ��isr���жϿ�����Ӳ��
	intc_setvectaddr(KEY_EINT2, isr_eint2);
	intc_setvectaddr(KEY_EINT3, isr_eint3);
	intc_setvectaddr(KEY_EINT16_19, isr_eint16171819);
	
	// ʹ���ж�
	intc_enable(KEY_EINT2);
	intc_enable(KEY_EINT3);
	intc_enable(KEY_EINT16_19);
	// ������Ӹ�����
	//while (1)
	//{
	//	printf("B ");
	//	delay(100);
	//}

	*/
	return 0;
}

