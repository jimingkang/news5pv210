#include <main.h>
#include <s5pv210-irq.h>

#define S5PV210_EXT_INT0_CON 	 	(0xE0200000 + 0x0E00)
#define S5PV210_EXT_INT0_MASK 	 	(0xE0200000 + 0x0F00)
#define S5PV210_EXT_INT0_PEND 	 	(0xE0200000 + 0x0F40)

/*
 * EINT2 interrupt function
 */
static void eint2_interrupt_func(void * data)
{
	serial_printf(0, "EINT2 interrupt\r\n");

	if((readl(S5PV210_GPH0DAT) & (0x1<<2)) == 0)
		led_set_status(LED_NAME_LED1, LED_STATUS_ON);
	else
		led_set_status(LED_NAME_LED1, LED_STATUS_OFF);

	/* clear interrupt status bit */
	writel(S5PV210_EXT_INT0_PEND, (readl(S5PV210_EXT_INT0_PEND) & ~(0x1<<2)) | (0x1<<2));
}

int tester_key_interrupt(int argc, char * argv[])
{
	/* LEFT BUTTON - GPH0_2 */

	/* Config to EXT_INT[2] */
	writel(S5PV210_GPH0CON, (readl(S5PV210_GPH0CON) & ~(0xf<<8)) | (0xf<<8));
	/* Pull up */
	writel(S5PV210_GPH0PUD, (readl(S5PV210_GPH0PUD) & ~(0x3<<4)) | (0x2<<4));
	/* Both edge triggered */
	writel(S5PV210_EXT_INT0_CON, (readl(S5PV210_EXT_INT0_CON) & ~(0x7<<8)) | (0x4<<8));
	/* Enables Interrupt */
	writel(S5PV210_EXT_INT0_MASK, (readl(S5PV210_EXT_INT0_MASK) & ~(0x1<<2)) | (0x0<<2));

	/* Request EINT2 interrupt */
	request_irq("EINT2", eint2_interrupt_func, 0);

	while(1)
	{
	}
	return 0;
}
