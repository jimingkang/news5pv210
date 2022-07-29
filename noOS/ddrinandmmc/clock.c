#include "clock.h"
#include "cpu_base.h"
#include "debug.h"
// 时钟控制器基地址
#define ELFIN_CLOCK_POWER_BASE		0xE0100000	

// 时钟相关的寄存器相对时钟控制器基地址的偏移值
#define APLL_LOCK_OFFSET		0x00		
#define MPLL_LOCK_OFFSET		0x08

#define APLL_CON0_OFFSET		0x100
#define APLL_CON1_OFFSET		0x104
#define MPLL_CON_OFFSET			0x108

#define CLK_SRC0_OFFSET			0x200
#define CLK_SRC1_OFFSET			0x204
#define CLK_SRC2_OFFSET			0x208
#define CLK_SRC3_OFFSET			0x20c
#define CLK_SRC4_OFFSET			0x210
#define CLK_SRC5_OFFSET			0x214
#define CLK_SRC6_OFFSET			0x218
#define CLK_SRC_MASK0_OFFSET	0x280
#define CLK_SRC_MASK1_OFFSET	0x284

#define CLK_DIV0_OFFSET			0x300
#define CLK_DIV1_OFFSET			0x304
#define CLK_DIV2_OFFSET			0x308
#define CLK_DIV3_OFFSET			0x30c
#define CLK_DIV4_OFFSET			0x310
#define CLK_DIV5_OFFSET			0x314
#define CLK_DIV6_OFFSET			0x318
#define CLK_DIV7_OFFSET			0x31c

#define CLK_DIV0_MASK			0x7fffffff

// 这些M、P、S的配置值都是查数据手册中典型时钟配置值的推荐配置得来的。
// 这些配置值是三星推荐的，因此工作最稳定。如果是自己随便瞎拼凑出来的那就要
// 经过严格测试，才能保证一定对。
#define APLL_MDIV      	 		0x7d		// 125
#define APLL_PDIV       		0x3
#define APLL_SDIV       		0x1

#define MPLL_MDIV				0x29b		// 667
#define MPLL_PDIV				0xc
#define MPLL_SDIV				0x1

#define set_pll(mdiv, pdiv, sdiv)	(1<<31 | mdiv<<16 | pdiv<<8 | sdiv)
#define APLL_VAL			set_pll(APLL_MDIV,APLL_PDIV,APLL_SDIV)
#define MPLL_VAL			set_pll(MPLL_MDIV,MPLL_PDIV,MPLL_SDIV)


#define REG_CLK_SRC0	(ELFIN_CLOCK_POWER_BASE + CLK_SRC0_OFFSET)
#define REG_APLL_LOCK	(ELFIN_CLOCK_POWER_BASE + APLL_LOCK_OFFSET)
#define REG_MPLL_LOCK	(ELFIN_CLOCK_POWER_BASE + MPLL_LOCK_OFFSET)
#define REG_CLK_DIV0	(ELFIN_CLOCK_POWER_BASE + CLK_DIV0_OFFSET)
#define REG_APLL_CON0	(ELFIN_CLOCK_POWER_BASE + APLL_CON0_OFFSET)
#define REG_MPLL_CON	(ELFIN_CLOCK_POWER_BASE + MPLL_CON_OFFSET)

#define rREG_CLK_SRC0	(*(volatile unsigned int *)REG_CLK_SRC0)
#define rREG_APLL_LOCK	(*(volatile unsigned int *)REG_APLL_LOCK)
#define rREG_MPLL_LOCK	(*(volatile unsigned int *)REG_MPLL_LOCK)
#define rREG_CLK_DIV0	(*(volatile unsigned int *)REG_CLK_DIV0)
#define rREG_APLL_CON0	(*(volatile unsigned int *)REG_APLL_CON0)
#define rREG_MPLL_CON	(*(volatile unsigned int *)REG_MPLL_CON)


void clock_init(void)
{
	// 1 设置各种时钟开关，暂时不使用PLL
	rREG_CLK_SRC0 = 0x0;
	
	// 2 设置锁定时间，使用默认值即可
	// 设置PLL后，时钟从Fin提升到目标频率时，需要一定的时间，即锁定时间
	rREG_APLL_LOCK = 0x0000ffff;
	rREG_MPLL_LOCK = 0x0000ffff;
	
	// 3 设置分频
	// 清bit[0~31]
	rREG_CLK_DIV0 = 0x14131440;
	
	// 4 设置PLL
	// FOUT = MDIV*FIN/(PDIV*2^(SDIV-1))=0x7d*24/(0x3*2^(1-1))=1000 MHz
	rREG_APLL_CON0 = APLL_VAL;
	// FOUT = MDIV*FIN/(PDIV*2^SDIV)=0x29b*24/(0xc*2^1)= 667 MHz
	rREG_MPLL_CON = MPLL_VAL;
	
	// 5 设置各种时钟开关,使用PLL
	rREG_CLK_SRC0 = 0x10001111;
}
//addfrom https://github.com/rocky-wang/bare_smart210/blob/master/clock.c

static unsigned long s5pv210_get_pll_clk(int pllreg)
{
	struct s5pv210_clock *clk = (struct s5pv210_clock *)S5PV210_CLOCK_BASE;
	unsigned long r, m, p, s, mask, fout;
	unsigned int freq;

	switch (pllreg) {
	case APLL:
		r = readl(&clk->apll_con);
		break;
	case MPLL:
		r = readl(&clk->mpll_con);
		break;
	case EPLL:
		r = readl(&clk->epll_con);
		break;
	case VPLL:
		r = readl(&clk->vpll_con);
		break;
	default:
		INFO("Unsupported PLL (%d)\n", pllreg);
		return 0;
	}

	/*
	 * APLL_CON: MIDV [25:16]
	 * MPLL_CON: MIDV [25:16]
	 * EPLL_CON: MIDV [24:16]
	 * VPLL_CON: MIDV [24:16]
	 */
	if (pllreg == APLL || pllreg == MPLL)
		mask = 0x3ff;
	else
		mask = 0x1ff;

	m = (r >> 16) & mask;

	/* PDIV [13:8] */
	p = (r >> 8) & 0x3f;
	/* SDIV [2:0] */
	s = r & 0x7;

	freq = CONFIG_SYS_CLK_FREQ_C110;
	if (pllreg == APLL) {
		if (s < 1)
			s = 1;
		/* FOUT = MDIV * FIN / (PDIV * 2^(SDIV - 1)) */
		fout = m * (freq / (p * (1 << (s - 1))));
	} else
		/* FOUT = MDIV * FIN / (PDIV * 2^SDIV) */
		fout = m * (freq / (p * (1 << s)));

	return fout;
}

unsigned long get_pll_clk(int pllreg)
{
    return s5pv210_get_pll_clk(pllreg);
}

/* s5pv210: return ARM clock frequency */
static unsigned long s5pv210_get_arm_clk(void)
{
	struct s5pv210_clock *clk =(struct s5pv210_clock *)S5PV210_CLOCK_BASE;
	unsigned long div;
	unsigned long dout_apll, armclk;
	unsigned int apll_ratio;

	div = readl(&clk->div0);

	/* APLL_RATIO: [2:0] */
	apll_ratio = div & 0x7;

	dout_apll = get_pll_clk(APLL) / (apll_ratio + 1);
	armclk = dout_apll;

	return armclk;
}

unsigned long get_arm_clk(void)
{
	return s5pv210_get_arm_clk();
}

static unsigned long get_hclk(void)
{
	struct s5pv210_clock *clk =(struct s5pv210_clock *)S5PV210_CLOCK_BASE;
	unsigned long hclkd0;
	unsigned int div, d0_bus_ratio;

	div = readl(&clk->div0);
	/* D0_BUS_RATIO: [10:8] */
	d0_bus_ratio = (div >> 8) & 0x7;

	hclkd0 = get_arm_clk() / (d0_bus_ratio + 1);

	return hclkd0;
}

static unsigned long get_hclk_sys(int dom)
{
	struct s5pv210_clock *clk =(struct s5pv210_clock *)S5PV210_CLOCK_BASE;
	unsigned long hclk;
	unsigned int div;
	unsigned int offset;
	unsigned int hclk_sys_ratio;

	if (dom == CLK_M)
		return get_hclk();

	div = readl(&clk->div0);

	/*
	 * HCLK_MSYS_RATIO: [10:8]
	 * HCLK_DSYS_RATIO: [19:16]
	 * HCLK_PSYS_RATIO: [27:24]
	 */
	offset = 8 + (dom << 0x3);

	hclk_sys_ratio = (div >> offset) & 0xf;

	hclk = get_pll_clk(MPLL) / (hclk_sys_ratio + 1);

	return hclk;
}

static unsigned long get_pclk_sys(int dom)
{
	struct s5pv210_clock *clk =(struct s5pv210_clock *)S5PV210_CLOCK_BASE;
	unsigned long pclk;
	unsigned int div;
	unsigned int offset;
	unsigned int pclk_sys_ratio;

	div = readl(&clk->div0);

	/*
	 * PCLK_MSYS_RATIO: [14:12]
	 * PCLK_DSYS_RATIO: [22:20]
	 * PCLK_PSYS_RATIO: [30:28]
	 */
	offset = 12 + (dom << 0x3);

	pclk_sys_ratio = (div >> offset) & 0x7;

	pclk = get_hclk_sys(dom) / (pclk_sys_ratio + 1);

	return pclk;
}

static unsigned long s5pv210_get_pclk(void)
{
	return get_pclk_sys(CLK_P);
}

static unsigned long s5pv210_get_uart_clk(int dev_index)
{
    return s5pv210_get_pclk();
}

unsigned long get_uart_clk(int dev_index)
{
	return s5pv210_get_uart_clk(dev_index);
}











