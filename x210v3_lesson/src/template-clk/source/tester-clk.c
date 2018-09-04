#include <main.h>
#include <s5pv210/reg-wdg.h>

int tester_clk(int argc, char * argv[])
{
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

	serial_printf(0, "******************************************\r\n");
	serial_printf(0, "x210 clk test...\r\n\r\n");

	serial_printf(0, "armclk = %dMHz\r\n",armclk/1000000);
	serial_printf(0, "msys-hclk = %dMHz\r\n",msys_hclk/1000000);
	serial_printf(0, "msys-pclk = %dMHz\r\n",msys_pclk/1000000);
	serial_printf(0, "dsys-hclk = %dMHz\r\n",dsys_hclk/1000000);
	serial_printf(0, "dsys-pclk = %dMHz\r\n",dsys_pclk/1000000);
	serial_printf(0, "psys-hclk = %dMHz\r\n",psys_hclk/1000000);
	serial_printf(0, "psys-pclk = %dMHz\r\n",psys_pclk/1000000);
	serial_printf(0, "******************************************\r\n");

	while(1);

	return 0;
}
