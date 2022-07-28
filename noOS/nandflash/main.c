#include "stdio.h"
#include "main.h"
#include "int.h"

#include "debug.h"
#include "nand.h"
void uart_init(void);


#define KEY_EINT2		NUM_EINT2		// left
#define KEY_EINT3		NUM_EINT3		// down
#define KEY_EINT16_19	NUM_EINT16_31	// ����4�����õ�

// nand_test ʹ�õ���BLOCK_NO���е���ҳ
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
    // ���Ȳ�������д��
    if ((ret = nand_erase(PAGE_ADDR)) == 0)
        printf("success to erase block %d\r\n", BLOCK_NO);
    else
        printf("fail to erase block %d\r\n", BLOCK_NO);
    #endif


    // �ȸ�Ѿrandomд4��byte
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

    // Ȼ���������ַ����������������Բ��Ե���
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
