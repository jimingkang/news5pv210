#include "stdio.h"
#include "main.h"
#include "int.h"
void uart_init(void);

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
        
	unsigned int num=0;

uart_init();
irq_init();
dm9000_init();
// key_init_interrupt();
printf(" jump to main \r\n");	
      
//	key_init();
//	key_polling();
	dm9000_arp();
	
	while(1)
		{
			printf("\r\n");
			printf("========= Boot for S5pv210 Main Menu =========\r\n");
			printf("=========  designed by KungfuMonkey  =========\r\n");
			printf("\r\n");
			printf("1.ARP Request for PC MAC\r\n");
			printf("2.Download Linux Kernel to SDRAM\r\n");
			printf("3.Run the linux\r\n");
			
			scanf("%d\r\n",&num);
			printf("%d\r\n",num);
			
			
			switch(num)
			{
				case 1:
					arp_request();
					break;
					
				case 2:
					tftp_send_request("zImage");
					break;
					
				case 3:
				//boot_linux();					
					break;

					
				default:
					printf("Error: wrong selection!\r\n");
					break;
			}
		}


	return 0;
}
