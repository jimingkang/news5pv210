#include <main.h>
#include <s5pv210/reg-wdg.h>

int tester_watchdog_timer(int argc, char * argv[])
{
/*
	u64_t armclk,msys_hclk,msys_pclk,dsys_hclk,dsys_pclk,psys_hclk,psys_pclk;
	if(!clk_get_rate("armclk", &armclk))
		return 0;
	if(!clk_get_rate("msys-hclk", &msys_hclk))
		return 0;
	if(!clk_get_rate("msys-pclk", &msys_pclk))
		return 0;
	if(!clk_get_rate("dsys-hclk", &dsys_hclk))
		return 0;
	if(!clk_get_rate("dsys-pclk", &dsys_pclk))
		return 0;
	if(!clk_get_rate("psys-hclk", &psys_hclk))
		return 0;
	if(!clk_get_rate("psys-pclk", &psys_pclk))
		return 0;

	serial_printf(0, "armclk = %d\r\n",armclk/1000000);
	serial_printf(0, "msys-hclk = %d\r\n",msys_hclk/1000000);
	serial_printf(0, "msys-pclk = %d\r\n",msys_pclk/1000000);
	serial_printf(0, "dsys-hclk = %d\r\n",dsys_hclk/1000000);
	serial_printf(0, "dsys-pclk = %d\r\n",dsys_pclk/1000000);
	serial_printf(0, "psys-hclk = %d\r\n",psys_hclk/1000000);
	serial_printf(0, "psys-pclk = %d\r\n",psys_pclk/1000000);
*/
	serial_printf(0, "S5PV210 WatchDog test...\r\n\r\n");

	serial_printf(0, "start watchdog now!\r\n");
	serial_printf(0, "4s later, System will reboot\r\n");
	writel(S5PV210_WTCON, 0x0000);
	writel(S5PV210_WTCNT, 0xffff);
	writel(S5PV210_WTCON, 0xff21);

	serial_printf(0, "reboot 0 seconds\r\n");
	mdelay(1000);
	serial_printf(0, "reboot 1 seconds\r\n");
	mdelay(1000);
	serial_printf(0, "reboot 2 seconds\r\n");
	mdelay(1000);
	serial_printf(0, "reboot 3 seconds\r\n");
	writel(S5PV210_WTCNT, 0xffff);
	mdelay(1000);
	serial_printf(0, "reboot 4 seconds\r\n");
	mdelay(1000);
	serial_printf(0, "reboot 5 seconds\r\n");
	mdelay(1000);
	serial_printf(0, "reboot 6 seconds\r\n");
	mdelay(1000);
	serial_printf(0, "reboot 7 seconds\r\n");
	mdelay(1000);
	serial_printf(0, "reboot 8 seconds\r\n");
	mdelay(1000);
	serial_printf(0, "reboot 9 seconds\r\n");
	while(1)
	{
	}

	return 0;
}
