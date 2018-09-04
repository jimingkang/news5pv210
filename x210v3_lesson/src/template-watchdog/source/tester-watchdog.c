#include <main.h>
#include <s5pv210/reg-wdg.h>

int tester_watchdog(int argc, char * argv[])
{
	serial_printf(0, "S5PV210 WatchDog test...\r\n");
	serial_printf(0, "Five minutes later, System will reboot\r\n");

	serial_printf(0, "reboot 5 seconds later\r\n");
	mdelay(1000);
	serial_printf(0, "reboot 4 seconds later\r\n");
	mdelay(1000);
	serial_printf(0, "reboot 3 seconds later\r\n");
	mdelay(1000);
	serial_printf(0, "reboot 2 seconds later\r\n");
	mdelay(1000);
	serial_printf(0, "reboot 1 seconds later\r\n");
	mdelay(1000);

	serial_printf(0, "reboot 0 seconds, System reboot...\r\n");
	writel(S5PV210_WTCON, 0x0000);
	writel(S5PV210_WTCNT, 0x0001/*0xffff*/);
	writel(S5PV210_WTCON, 0x0021/*0xff21*/);
/*
	serial_printf(0, "reboot 0 seconds\r\n");
	mdelay(1000);
	serial_printf(0, "reboot 1 seconds\r\n");
	mdelay(1000);
	serial_printf(0, "reboot 2 seconds\r\n");
	mdelay(1000);
	serial_printf(0, "reboot 3 seconds\r\n");
	mdelay(1000);
	serial_printf(0, "reboot 3 seconds\r\n");
	mdelay(1000);
	serial_printf(0, "reboot 4 seconds\r\n");
	mdelay(1000);
	serial_printf(0, "reboot 5 seconds\r\n");
	mdelay(1000);
	serial_printf(0, "reboot 6 seconds\r\n");
*/
	while(1)
	{
	}

	return 0;
}
