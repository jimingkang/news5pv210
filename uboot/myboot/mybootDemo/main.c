
#define WIDTHEIGHT	480
#define HEIGHT	272


int main()
{
	unsigned int num;

	
	unsigned char ret = 0;
	
	#ifdef  MMU_ON
	mmu_init();
	#endif
	led_init();
	key_init();
	irq_init();
	uart0_init();
	lcd_init();	
	i2c0_init();
	
		
	lcd_clear_screen(0x000000);
	dm9000_init();
	
	printf("First write address of 0x0,");
	at24cxx_write(0x0, 0xAA);  //往设备内存0地址写入0xAA
	ret = at24cxx_read(0x0);   //读设备内存0地址里面的数据
	printf("the value in address 0x0 = %x\r\n",ret);
	
	printf("Second write address of 0x1,");
	at24cxx_write(0x1,0x33);
	ret = at24cxx_read(0x1);
	printf("the value in address 0x1 = %x\r\n",ret);
	
	
	
//	dm9000_arp();
	
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
				  boot_linux();					
					break;

					
				default:
					printf("Error: wrong selection!\r\n");
					break;
			}
		}
	return 0;
}
