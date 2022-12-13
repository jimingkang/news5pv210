#include "stdio.h"
#include "main.h"
#include "int.h"
void uart_init(void);


#define KEY_EINT2		NUM_EINT2		// left
#define KEY_EINT3		NUM_EINT3		// down
#define KEY_EINT16_19	NUM_EINT16_31	// ����4�����õ�
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
        unsigned int num=2;


	uart_init();
        printf("after uart init \r\n");	
      

	printf("after irq init \r\n");		
	
         //key_init();
         //key_polling();


	
/**/
 
       key_init_interrupt();
	dm9000_init();
	// ���������Ҫʹ���жϣ���Ҫ�����жϳ�ʼ����������ʼ���жϿ�����
	system_init_exception();
	
	printf("-------------key interrypt test--------------");
	
	// ��isr���жϿ�����Ӳ��
	intc_setvectaddr(KEY_EINT2, isr_eint2);
	intc_setvectaddr(KEY_EINT3, isr_eint3);
	intc_setvectaddr(KEY_EINT16_19, isr_eint16171819);

        intc_setvectaddr(10,dm9000_isr);

	  printf("dm9000_isr addr=%x\r\n",(int)dm9000_isr);

	// ʹ���ж�
	intc_enable(KEY_EINT2);
	intc_enable(KEY_EINT3);
	intc_enable(KEY_EINT16_19);
        
intc_enable(10);

	// ������Ӹ�����
	//while (1)
	//{
	//	printf("A");
	//	delay(100);
	//}

//dm9000_arp();
if(1)
		{
			printf("\r\n");
			printf("========= Boot for S5pv210 Main Menu =========\r\n");
			printf("=========  designed by jimmy  =========\r\n");
			printf("\r\n");
			printf("1.ARP Request for PC MAC\r\n");
			printf("2.Download Linux Kernel to SDRAM\r\n");
			printf("3.Run the linux\r\n");
			
			//scanf("%d\r\n",&num);
			printf("%d\r\n",num);
			
			
			switch(num)
			{
				case 1:
					arp_request();
					break;
					
				case 2:

					tftp_send_request("test");
					break;
					
				case 3:
				// boot_linux();					
					break;

					
				default:
					printf("Error: wrong selection!\r\n");
					break;
			}
		}

	return 0;
}
