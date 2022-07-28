#define EXT_INT_0_CON   (*(volatile unsigned long*) 0xE0200E00)
#define EXT_INT_0_MASK  (*(volatile unsigned long*) 0xE0200F00)
#define EXT_INT_0_PEND  (*(volatile unsigned long*) 0xe0200f40)
#define EXT_INT_2_CON   (*(volatile unsigned long*) 0xe0200e08)
#define EXT_INT_2_MASK  (*(volatile unsigned long*) 0xe0200f08)
#define EXT_INT_2_PEND  (*(volatile unsigned long*) 0xe0200f48)
#define VIC0INTENABLE   (*(volatile unsigned long*) 0xf2000010)
#define VIC0VECTADDR7   (*(volatile unsigned long*) 0xF200011C)
#define VIC0VECTADDR16  (*(volatile unsigned long*) 0xf2000140)
#define VIC0ADDRESS     (*(volatile unsigned long*) 0xf2000f00)
#define VIC1ADDRESS     (*(volatile unsigned long*) 0xf2100f00)
#define VIC2ADDRESS  		(*(volatile unsigned long*) 0xF2200F00)
#define VIC3ADDRESS  		(*(volatile unsigned long*) 0xF2300F00)
#define VIC1INTENABLE   (*(volatile unsigned long*) 0xf2100010)
#define VIC1VECTADDR14  (*(volatile unsigned long*) 0xf2100138)
#define VIC0INTSELECT   (*(volatile unsigned long*) 0xf200000c)
#define VIC1INTSELECT   (*(volatile unsigned long*) 0xf210000c)

#define pExceptionIRQ   (*(volatile unsigned long*) 0xd0037418)

#define VIC0IRQSTATUS   (*(volatile unsigned long*) 0xf2000000)
#define VIC1IRQSTATUS   (*(volatile unsigned long*) 0xf2100000)

extern void i2c0_isr(void);
extern void dm9000_isr(void);
extern void irq();

void key_isr()
{
	volatile unsigned int val;

	//判断是extint16 还是extint17
	val = EXT_INT_2_PEND&0b11;
	
	switch(val)
	{
		//Key1
		case 1:  
			led_flow_water();
			break;
		//Key2
		case 2: 
			led_flash();
			break;
			
		default:
			break;	
		
	}
	//中断清除
	EXT_INT_2_PEND = ~0x0;
	//响应中断函数地址清除
	VIC0ADDRESS = 0;
	VIC1ADDRESS = 0;
	VIC2ADDRESS = 0;
	VIC3ADDRESS = 0;

}

void irqhandler()
{
printf("enter irqhandler");
	/**
	*1.取出中断服务程序地址
	*2.跳转到正真的中断服务程序函数去
	*/
	void (*key_isr)(void);
	void (*i2c0_isr)(void);
	void (*dm9000_isr)(void);
	
	if(VIC0IRQSTATUS != 0)
		{
			if((VIC0IRQSTATUS&(0x1<<16))||(VIC0IRQSTATUS&(0x1<<17)))
				{
					key_isr = (void (*)(void))VIC0ADDRESS;
					(*key_isr)();	
				}
			if(VIC0IRQSTATUS&(0x1<<7))
				{
					dm9000_isr = (void (*)(void))VIC0ADDRESS;
					(*dm9000_isr)();
				}
		}
	else if(VIC1IRQSTATUS != 0)
		{
			i2c0_isr = (void (*)(void))VIC1ADDRESS;
			(*i2c0_isr)();
		}	
}
void irq_init()
{
	EXT_INT_0_CON &= ~(0xFF<<8);
	EXT_INT_0_CON |= (0x2<<8)|(0x2<<12);
	EXT_INT_2_CON &= ~(0xFFFF<<0);
	EXT_INT_2_CON = 0x2|(0x2<<4)|(2<<8)|(2<<12);
	
	EXT_INT_0_MASK &= ~(3<<2);			// 外部中断允许
	EXT_INT_2_MASK &= ~(0x0f<<0);

	// 4. 清挂起，清除是写1，不是写0
	EXT_INT_0_PEND |= (3<<2);
	EXT_INT_2_PEND |= (0x0F<<0);

	pExceptionIRQ = (unsigned long)irq;
	
	/*extint16,extint17设置下降沿触发(K1,K2)*/
	//EXT_INT_2_CON = 0x2|(0x2<<4);
	
	EXT_INT_2_CON = 0x2|(0x2<<4);
	/*extint7高电平触发(DM9000)*/
	EXT_INT_0_CON &= (~(0x7<<28));
	EXT_INT_0_CON |= (0x1<<28);
	
	/*mask寄存器设置*/
	EXT_INT_0_MASK = 0x0;
	EXT_INT_2_MASK = 0;




	
	/*mode寄存器设置，可省略，默认为irq模式*/
	VIC0INTSELECT &= (~(1<<7));
	VIC0INTSELECT &= ~(1<<16);
	VIC1INTSELECT &= ~(1<<14);
	
	/*中断使能*/
	VIC0INTENABLE = (0x1<<16)|(0x1<<7);					/*extint16~31,extint7*/
	VIC1INTENABLE = (0x1<<14);          				/*I2C*/
	
	/*中断向量地址*/
	
	VIC0VECTADDR16 = (int)key_isr;    /*由于extint16 与extint17公用一个中断向量表项*/
	
	VIC1VECTADDR14 = (int)i2c0_isr;
	
	VIC0VECTADDR7 = (int)dm9000_isr;

	
	/*使能irq中断(cpsr设置)*/
	__asm__(
		"mrs r0,cpsr\n"
		"bic r0,r0,#0x80\n"
		"msr cpsr_c,r0\n"
		:
		:
	);
		
}
