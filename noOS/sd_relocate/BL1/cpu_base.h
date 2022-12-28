#ifndef CPU_BASE
#define CPU_BASE

#define S5PV210_WATCHDOG_BASE   0xE2700000
#define S5PV210_CLOCK_BASE      0xE0100000
#define S5PV210_GPIO_BASE 		0xE0200000
#define S5PV210_GPH2_BASE		0xE0200C40

/* Time Value */
/* APLL : 1000MHZ */
#define APLL_MDIV   125
#define APLL_PDIV   3
#define APLL_SDIV   1

/* MPLL : 667MHZ */
#define MPLL_MDIV   667
#define MPLL_PDIV   12
#define MPLL_SDIV   1

/* EPLL : 96MHZ */
#define EPLL_MDIV   96	
#define EPLL_PDIV	6
#define EPLL_SDIV	2

/* VPLL : 54MHZ */
#define VPLL_MDIV	108
#define VPLL_PDIV	6
#define VPLL_SDIV	3

/* XPLL Value */
#define APLL_VALUE  ((1<<31)|(APLL_MDIV<<16)|(APLL_PDIV<<8)|(APLL_SDIV<<0))
#define MPLL_VALUE  ((1<<31)|(MPLL_MDIV<<16)|(MPLL_PDIV<<8)|(MPLL_SDIV<<0))
#define EPLL_VALUE  ((1<<31)|(EPLL_MDIV<<16)|(EPLL_PDIV<<8)|(EPLL_SDIV<<0))
#define VPLL_VALUE  ((1<<31)|(VPLL_MDIV<<16)|(VPLL_PDIV<<8)|(VPLL_SDIV<<0))

/* CLK_DIV */
#define CLK_DIV0_VAL    ((0x1<<28)|(0x4<<24)|(0x1<<20)|(0x3<<16)|(0x1<<12)|(0x4<<8)|(0x3<<4)|(0x0<<0))

/* CLK SRC */
#define CLK_SRC0_VAR    ((0x1<<28)|(0x1<<12)|(0x1<<8)|(0x1<<4)|(0x1<<0))

#define CLK_GATE_IP1    0xE0100464

#define	__REG(x)		(*(volatile unsigned int *)(x))

#define readb(a)		(*(volatile unsigned char *)(a))
#define readw(a)		(*(volatile unsigned short *)(a))
#define readl(a)		(*(volatile unsigned int *)(a))

#define writeb(v,a)		(*(volatile unsigned char *)(a) = (v))
#define writew(v,a)		(*(volatile unsigned short *)(a) = (v))
#define writel(v,a)		(*(volatile unsigned int *)(a) = (v))

#endif

