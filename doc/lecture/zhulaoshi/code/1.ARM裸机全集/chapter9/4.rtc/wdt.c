
#define		WTCON		(0xE2700000)
#define		WTDAT		(0xE2700004)
#define		WTCNT		(0xE2700008)
#define 	WTCLRINT	(0xE270000C)

#define 	rWTCON		(*(volatile unsigned int *)WTCON)
#define 	rWTDAT		(*(volatile unsigned int *)WTDAT)
#define 	rWTCNT		(*(volatile unsigned int *)WTCNT)
#define 	rWTCLRINT	(*(volatile unsigned int *)WTCLRINT)


// 初始化WDT使之可以产生中断
void wdt_init_reset(void)
{
	// 第一步，设置好预分频器和分频器，得到时钟周期是128us
	rWTCON &= ~(0xff<<8);
	rWTCON |= (65<<8);				// 1MHz
	
	rWTCON &= ~(3<<3);
	rWTCON |= (3<<3);				// 1/128 MHz, T = 128us
	
	// 第二步，设置中断和复位信号的使能或禁止
	rWTCON &= ~(1<<2);				// disable wdt interrupt
	rWTCON |= (1<<0);				// enable wdt reset
	
	// 第三步，设置定时时间
	// WDT定时计数个数，最终定时时间为这里的值×时钟周期
	rWTDAT = 10000;					// 定时1.28s
	rWTCNT = 10000;					// 定时1.28s
	
	// 其实WTDAT中的值不会自动刷到WTCNT中去，如果不显式设置WTCON中的值，它的值就是
	// 默认值，然后以这个默认值开始计数，所以这个时间比较久。如果我们自己显式的
	// 设置了WTCNT和WTDAT一样的值，则第一次的定时值就和后面的一样了。
	//rWTDAT = 1000;					// 定时0.128s
	//rWTCNT = 1000;					// 定时0.128s
	
	// 第四步，先把所有寄存器都设置好之后，再去开看门狗
	rWTCON |= (1<<5);				// enable wdt
}















