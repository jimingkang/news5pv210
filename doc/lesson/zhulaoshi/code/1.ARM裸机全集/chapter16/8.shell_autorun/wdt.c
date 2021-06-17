#include "int.h"
#include "shell.h"

#define		WTCON		(0xE2700000)
#define		WTDAT		(0xE2700004)
#define		WTCNT		(0xE2700008)
#define 	WTCLRINT	(0xE270000C)

#define 	rWTCON		(*(volatile unsigned int *)WTCON)
#define 	rWTDAT		(*(volatile unsigned int *)WTDAT)
#define 	rWTCNT		(*(volatile unsigned int *)WTCNT)
#define 	rWTCLRINT	(*(volatile unsigned int *)WTCLRINT)


// 初始化WDT使之可以产生中断
void wdt_init_interrupt(void)
{
	// 第一步，设置好预分频器和分频器，得到时钟周期是128us
	rWTCON &= ~(0xff<<8);
	rWTCON |= (65<<8);				// 1MHz
	
	rWTCON &= ~(3<<3);
	rWTCON |= (3<<3);				// 1/128 MHz, T = 128us
	
	// 第二步，设置中断和复位信号的使能或禁止
	rWTCON |= (1<<2);				// enable wdt interrupt
	rWTCON &= ~(1<<0);				// disable wdt reset
	
	// 第三步，设置定时时间
	// WDT定时计数个数，最终定时时间为这里的值×时钟周期
	//rWTDAT = 10000;					// 定时1.28s
	//rWTCNT = 10000;					// 定时1.28s
	
	// 其实WTDAT中的值不会自动刷到WTCNT中去，如果不显式设置WTCON中的值，它的值就是
	// 默认值，然后以这个默认值开始计数，所以这个时间比较久。如果我们自己显式的
	// 设置了WTCNT和WTDAT一样的值，则第一次的定时值就和后面的一样了。
	//rWTDAT = 1000;					// 定时0.128s
	//rWTCNT = 1000;					// 定时0.128s
	
	rWTDAT = 7812;						// 定时1s
	rWTCNT = 7812;						// 定时1s

	// 第四步，先把所有寄存器都设置好之后，再去开看门狗
	rWTCON |= (1<<5);				// enable wdt
}


// wdt的中断处理程序
void isr_wdt(void)
{
	static int i = 0;
	// 看门狗定时器时间到了时候应该做的有意义的事情
	//printf("wdt interrupt, i = %d...", i++);
	// 计时，然后时间没到的时候在屏幕上打印倒数计数，时间到了自动执行命令
	// 执行完命令进入shell的死循环
	g_bootdelay--;
	putchar('\b');
	printf("%d", g_bootdelay);
	
	if (g_bootdelay == 0)
	{
		g_isgo = 1;
		
		// 把要自动执行的命令添加到这里，但是这里是中断处理程序，不适合执行长代码
		// 所以放在外面要好一些
		
		//printf("g_isgo = 1.\n");
		// 关闭wdt
		//intc_disable(NUM_WDT);
	}

	// 清中断
	intc_clearvectaddr();
	rWTCLRINT = 1;
}


// wdt作为定时来用的所有初始化
void wdt_timer_init(void)
{
	wdt_init_interrupt();
	// 如果程序中要使用中断，就要调用中断初始化来初步初始化中断控制器
	system_init_exception();
	intc_setvectaddr(NUM_WDT, isr_wdt);
	// 使能中断
	intc_enable(NUM_WDT);
}













