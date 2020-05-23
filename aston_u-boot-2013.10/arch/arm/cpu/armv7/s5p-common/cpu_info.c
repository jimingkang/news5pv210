/*
 * Copyright (C) 2009 Samsung Electronics
 * Minkyu Kang <mk7.kang@samsung.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include <common.h>
#include <asm/io.h>
#include <asm/arch/clk.h>

/* Default is s5pc100 */
unsigned int s5p_cpu_id = 0xC100;
/* Default is EVT1 */
unsigned int s5p_cpu_rev = 1;

#ifdef CONFIG_ARCH_CPU_INIT
int arch_cpu_init(void)
{
	s5p_set_cpu_id();

	return 0;
}
#endif

u32 get_device_type(void)
{
	return s5p_cpu_id;
}

#ifdef CONFIG_DISPLAY_CPUINFO
int print_cpuinfo(void)
{
	char buf[32];

/*	printf("CPU:\t%s%X@%sMHz\n",
			s5p_get_cpu_name(), s5p_cpu_id,
			strmhz(buf, get_arm_clk()));
*/
	printf("\nCPU:  S5PV210@%sMHz(OK)\n", strmhz(buf, get_arm_clk()));


/*
	printf("	APLL = %ldMHz, HclkMsys = %ldMHz, PclkMsys = %ldMHz\n",
				get_FCLK()/1000000, get_HCLK()/1000000, get_PCLK()/1000000);

	printf("	MPLL = %ldMHz, EPLL = %ldMHz\n",
				get_MPLL_CLK()/1000000, get_PLLCLK(EPLL)/1000000);
	printf("			   HclkDsys = %ldMHz, PclkDsys = %ldMHz\n",
				get_HCLKD()/1000000, get_PCLKD()/1000000);
	printf("			   HclkPsys = %ldMHz, PclkPsys = %ldMHz\n",
				get_HCLKP()/1000000, get_PCLKP()/1000000);
	printf("			   SCLKA2M	= %ldMHz\n", get_SCLKA2M()/1000000);
*/



	return 0;
}
#endif
