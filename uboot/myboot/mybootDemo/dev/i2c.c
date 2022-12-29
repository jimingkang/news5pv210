
#define GPD1CON  (*(volatile unsigned long*) 0xe02000c0)
#define GPD1PUD  (*(volatile unsigned long*) 0xe02000c8)
#define I2CCON0  (*(volatile unsigned long*) 0xe1800000)
#define I2CSTAT0 (*(volatile unsigned long*) 0xe1800004)
#define I2CDS0   (*(volatile unsigned char*) 0xe180000c)


#define VIC0ADDRESS     (*(volatile unsigned long*) 0xf2000f00)
#define VIC1ADDRESS     (*(volatile unsigned long*) 0xf2100f00)
#define VIC2ADDRESS  		(*(volatile unsigned long*) 0xF2200F00)
#define VIC3ADDRESS  		(*(volatile unsigned long*) 0xF2300F00)

#define I2C_WM     1               /*Tx Mode*/
#define I2C_RM     2							/*Rx Mode*/

typedef struct I2C
{
	unsigned char *Data;              /*���ݻ�����*/
	volatile int DataCount;						/*���ݳ���*/
	volatile int Mode;								/*��/д*/
	volatile int pt;									/*��������λ��*/
}t210_I2C;

static t210_I2C   my_t210_I2C;



void i2c0_init()
{
	/*��������,��ʼ��Ϊ����*/	
	GPD1CON |= 0x22;
	GPD1PUD |= 0x5;
	
	/*I2CCON����,bit[3:0]=0xf,PCLK=66.7MHz,PCLK=I2CLK/(bit[3:0]+1),I2CLK=4.1MHz*/
	I2CCON0 = (0x1<<7)|(0x0<<6)|(0x1<<5)|(0xf);

	
	/*I2CSTAT����,�������ʹ��*/
	I2CSTAT0 = 0x10;
	
	/*����������ӵ�irq_init()������*/
	/*�жϵ�ַ����*/

	/*irq�����ж�*/
}

/**
*Master Tx mode
*slvAddr: �ӻ���ַ��buf: ���ͻ��棬len: ���ͳ���
*/
void i2c0_write(unsigned char slvAddr,unsigned char *buf,int len)
{
		my_t210_I2C.Data = buf;
		my_t210_I2C.DataCount = len;
		my_t210_I2C.Mode = I2C_WM;
		my_t210_I2C.pt = 0;
	
		/*write slave address to I2CDS*/
		I2CDS0 = slvAddr;
		/*write 0xf0 to I2CSTAT*/
		I2CSTAT0 = 0xf0;		
		/*the data of the I2CDS is transmitted*/
	//	I2CCON0 &= ~(1<<4);
		
		
		while(my_t210_I2C.DataCount != -1) 
			;
}


/**
*Master Rx Mode
*
*/
void i2c0_read(unsigned char slvAddr,unsigned char *buf,int len)
{
	my_t210_I2C.Data = buf;
	my_t210_I2C.DataCount = len;
	my_t210_I2C.Mode = I2C_RM;
	my_t210_I2C.pt = -1;                    /*��ʾ��һ���жϲ��������ݣ�Ϊ��ַ����*/
	
	
	/*write slave address to I2CDS*/	
	I2CDS0 = slvAddr;
	/*write 0xb0 to I2CSTAT*/
	I2CSTAT0 = 0xb0;
	
//	I2CCON0 &= ~(1<<4);

	while(my_t210_I2C.DataCount !=0)
		;
		
}



/**
*I2C ACK period interrupt pending
*/
void i2c0_isr()
{
	
	unsigned int i;

	switch(my_t210_I2C.Mode)       
	{
		case I2C_WM :							//д�ж�
			{
				/*stop?*/
				/*Y*/
				if((my_t210_I2C.DataCount--) == 0)	
					{
						/*Write 0xD0 (M/T Stop) to I2CSTAT.*/	
						I2CSTAT0 = 0xd0;
						/*Clear pending bit .*/
						I2CCON0 = 0xaf;                   
						
						/*Wait until the stop condition takes effect.*/
						delay(10000);
						break;
					}
				/*N*/	
				else
					{
						/*Write new data transmitted to I2CDS.*/
						I2CDS0 = my_t210_I2C.Data[my_t210_I2C.pt++];
						
						/*Clear pending bit to resume.*/
						/*The data of the I2CDS is shifted to SDA.*/
						for(i=0;i<10;i++)
							;
						I2CCON0 = 0xaf;

						break;
					}							
			}	
			
		case I2C_RM :                           /*���ж�*/
			{
				/*�жϷ���ӦΪ==*/
				
				if(my_t210_I2C.pt == -1)	           /*��һ�β������ж�*/
					{
						my_t210_I2C.pt = 0;
						if(my_t210_I2C.DataCount ==1)
							I2CCON0 = 0x2f;                  /*��at24cxx�ֲ�֪����������ʱ�����һ�����ݲ���ҪACK*/
						else
							I2CCON0 = 0xaf;	
						break;
					}
				
				/*Read a new data from I2CDS.*/	
				my_t210_I2C.Data[my_t210_I2C.pt++] = I2CDS0;
				my_t210_I2C.DataCount --;
				
				if(my_t210_I2C.DataCount ==0)
					{
						/*Write 0x90 (M/R Stop) to I2CSTAT.*/	
						I2CSTAT0 = 0x90;
						/*Clear pending bit .*/
						I2CCON0 = 0xaf;
						
						/*Wait until the stop condition takes effect.*/
						delay(10000);
						break;
					}
				else
					{	
						/*Clear pending bit to resume.*/
						if(my_t210_I2C.DataCount ==1)
							I2CCON0 = 0x2f;                  /*��at24cxx�ֲ�֪����������ʱ�����һ�����ݲ���ҪACK*/
						else
							I2CCON0 = 0xaf;	
					}	
					break;	
			}
		default:
			break;
	}
	/*����ж�*/
	I2CCON0 &= ~(1<<4);
	
	/*����жϺ�����ַ*/
	VIC0ADDRESS = 0;
	VIC1ADDRESS = 0;
	VIC2ADDRESS = 0;
	VIC3ADDRESS = 0;
		
}



/* ��Ҫ����AT24CxxоƬ�ֲ�Ķ��ֽ�Э�� */
unsigned char at24cxx_read(unsigned char address)
{
	unsigned char val;
	i2c0_write(0xA0, &address, 1);	//AT24C02���豸��ַΪ0xa0
	i2c0_read(0xA0, (unsigned char *)&val, 1);
	return val;
}

/* ��Ҫ����AT24CxxоƬ�ֲ��д�ֽ�Э�� */
void at24cxx_write(unsigned char address, unsigned char data)
{
	unsigned char val[2];
	val[0] = address;
	val[1] = data;
	i2c0_write(0xA0, val, 2);
}


