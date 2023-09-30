#define 	GPJ2CON 	(*(volatile unsigned long *) 0xE0200280)
#define 	GPJ2DAT		(*(volatile unsigned long *) 0xE0200284)


/* ��ʱ���� */
void delay(int count)														
{
    while (count--)
        ;
}

void led_init()
{
	
	GPJ2CON=0x1111;	// ��������	
	
	GPJ2DAT=0b1111; //Ĭ�ϸߵ�ƽ���ر�led
	
}


void led_on()
{
	GPJ2DAT=0b1010;	
}


/* ��ˮ�ƺ��� */
void led_flow_water()															
{
	unsigned int i = 0;
	unsigned int j=0;
	for(j=0;j<8;j++)
	{		
		GPJ2DAT = ~(1<<i);	//GPJ2DAT��ֵ����Ϊ 1110B 1101B 1011B 0111B ,����ѭ��
		i++;											
		if (i == 4)
			i = 0;
		delay(0x100000);
	}
	GPJ2DAT=0b1111;	
}


void led_flash()
{
	unsigned int i;
	for(i=0;i<4;i++)
	{
		GPJ2DAT=0xf;	
		delay(1000000);
		GPJ2DAT=0x0;
		delay(1000000);
	}
	GPJ2DAT=0b1111;
}
