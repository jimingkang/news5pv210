#include <main.h>
#include <s5pv210/reg-others.h>

int tester_powerlock(int argc, char * argv[])
{
	serial_printf(0, "S5PV210 Powerlock test...\r\n");
	serial_printf(0, "Five minutes later, System will shutdown\r\n");

	serial_printf(0, "shutdown 5 seconds later\r\n");
	mdelay(1000);
	serial_printf(0, "shutdown 4 seconds later\r\n");
	mdelay(1000);
	serial_printf(0, "shutdown 3 seconds later\r\n");
	mdelay(1000);
	serial_printf(0, "shutdown 2 seconds later\r\n");
	mdelay(1000);
	serial_printf(0, "shutdown 1 seconds later\r\n");
	mdelay(1000);

	serial_printf(0, "0 seconds, System shutdown...\r\n");
	writel(S5PV210_PS_HOLD_CONTROL, (readl(S5PV210_PS_HOLD_CONTROL) & ~( 0x00000301 )) | ((0x1<<0) | (0x0<<8) | (0x0<<9)));

	while(1)
	{
	} 

	return 0;
}
