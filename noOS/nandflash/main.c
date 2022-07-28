#include "stdio.h"
#include "main.h"
#include "int.h"

#include "debug.h"
#include "nand.h"
void uart_init(void);


#define KEY_EINT2		NUM_EINT2		// left
#define KEY_EINT3		NUM_EINT3		// down
#define KEY_EINT16_19	NUM_EINT16_31	// 其余4个共用的

// nand_test 使用的是BLOCK_NO块中的首页
#define BLOCK_NO    10
#define PAGE_NO     (BLOCK_NO * NAND_BLOCK_SIZE)
#define PAGE_ADDR   (PAGE_NO * NAND_PAGE_SIZE)


#define OFFSET0 4
#define OFFSET1 5
#define OFFSET2 6
#define OFFSET3 7

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

/*
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
}*/

void nand_test(void)
{
    int ret = 0;
    unsigned char data1, data2, data3, data4;
    unsigned char buf[8];
    unsigned int bufInt[2];

    nand_init();
    nand_read_id();


    #if 0
    // 得先擦除才能写啊
    if ((ret = nand_erase(PAGE_ADDR)) == 0)
        printf("success to erase block %d\r\n", BLOCK_NO);
    else
        printf("fail to erase block %d\r\n", BLOCK_NO);
    #endif


    // 先给丫random写4个byte
    #if 1
    nand_random_write(PAGE_ADDR, OFFSET0, 'a');
    nand_random_write(PAGE_ADDR, OFFSET1, 'b');
    nand_random_write(PAGE_ADDR, OFFSET2, 'c');
    nand_random_write(PAGE_ADDR, OFFSET3, 'd');
    #endif

    #if 0
    nand_random_write(PAGE_ADDR, OFFSET0, 0xba);
    nand_random_write(PAGE_ADDR, OFFSET1, 0xde);
    nand_random_write(PAGE_ADDR, OFFSET2, 0xc0);
    nand_random_write(PAGE_ADDR, OFFSET3, 0xde);

    #endif

    // 然后再用三种方法，读出来看看对不对得上
    nand_random_read(PAGE_ADDR, OFFSET0, &data1);
    nand_random_read(PAGE_ADDR, OFFSET1, &data2);
    nand_random_read(PAGE_ADDR, OFFSET2, &data3);
    nand_random_read(PAGE_ADDR, OFFSET3, &data4);

    printf("PAGE_ADDR: \r\n", PAGE_ADDR);
    printf("4 byte data from nand_random_read: %x, %x, %x, %x\r\n", data1, data2, data3, data4);

    ret = nand_page_read(PAGE_ADDR, buf, sizeof(buf));
    if (ret != 0)
        printf("nand_page_read error!\r\n");
    else
        printf("4 byte data form nand_page_read: %x, %x, %x, %x\r\n", buf[OFFSET0], buf[OFFSET1], buf[OFFSET2], buf[OFFSET3]);

    ret = nand_page_read32(PAGE_ADDR, bufInt, sizeof(bufInt)/sizeof(unsigned int));
    if (ret != 0)
        printf("nand_page_read32 error!\r\n");
    else    
        printf("1 word data form nand_page_read32: %x\r\n", bufInt[OFFSET0]);
}

int main(void)
{
        

	uart_init();
	printf("after uart init \r\n");	
  
 	//nand_lowlevel_init();
    	//printf("the armclock is %d\n",get_arm_clk());
    	//nand_init_chip();
//nand_operation_test();



nand_test();

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
