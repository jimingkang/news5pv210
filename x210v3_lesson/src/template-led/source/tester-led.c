#include <main.h>

int tester_led(int argc, char * argv[])
{
	int index = 0;
	int flag = 0;

	while(1)
	{
		led_set_status(LED_NAME_LED1, LED_STATUS_OFF);
		led_set_status(LED_NAME_LED2, LED_STATUS_OFF);
		led_set_status(LED_NAME_LED3, LED_STATUS_OFF);
		led_set_status(LED_NAME_LED4, LED_STATUS_OFF);

		switch(index)
		{
		case 0:
			led_set_status(LED_NAME_LED1, LED_STATUS_ON);
			break;

		case 1:
			led_set_status(LED_NAME_LED2, LED_STATUS_ON);
			break;

		case 2:
			led_set_status(LED_NAME_LED3, LED_STATUS_ON);
			break;

		case 3:
			led_set_status(LED_NAME_LED4, LED_STATUS_ON);
			break;

		default:
			break;
		}

		mdelay(50);

		if(index == 0 || index == 3)
			flag = !flag;

		if(flag)
			index = (index + 1) % 4;
		else
			index = (index + 4 - 1) % 4;
	}

	return 0;
}
