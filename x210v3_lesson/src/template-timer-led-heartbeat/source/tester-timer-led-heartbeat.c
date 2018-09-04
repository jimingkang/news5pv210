#include <main.h>
#include <hw-led.h>

/*
 * Called by timer_interrupt in s5pv210-tick.c
 */
void led_heartbeat_task(void)
{
	static u32_t phase = 0;
	static u32_t period = 0;
	static u32_t delay = 0;
	static u32_t jiffies_old = 0;
	enum led_status status;
	u32_t timeout;

	timeout = jiffies_old + delay;
	if(time_before(jiffies, timeout))
		return;

	switch(phase)
	{
	case 0:
		period = 1260 * get_system_hz() / 1000;
		delay = 70 * get_system_hz() / 1000;
		phase++;
		status = LED_STATUS_ON;
		break;

	case 1:
		delay = period / 4 - (70 * get_system_hz() / 1000);
		phase++;
		status = LED_STATUS_OFF;
		break;

	case 2:
		delay = 70 * get_system_hz() / 1000;
		phase++;
		status = LED_STATUS_ON;
		break;

	default:
		delay = period - period / 4 - (70 * get_system_hz() / 1000);
		phase = 0;
		status = LED_STATUS_OFF;
		break;
	}

	led_set_status(LED_NAME_LED1, status);
	jiffies_old = jiffies;
}

int tester_timer_led_heartbeat(int argc, char * argv[])
{
	while(1)
	{
	}

	return 0;
}
