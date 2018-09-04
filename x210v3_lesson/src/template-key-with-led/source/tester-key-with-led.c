#include <main.h>

int tester_key_with_led(int argc, char * argv[])
{
	u32_t keyup, keydown;

	led_set_status(LED_NAME_LED1, LED_STATUS_OFF);
	led_set_status(LED_NAME_LED2, LED_STATUS_OFF);
	led_set_status(LED_NAME_LED3, LED_STATUS_OFF);
	led_set_status(LED_NAME_LED4, LED_STATUS_OFF);

	while(1)
	{
		if(get_key_event(&keyup, &keydown))
		{
			/* KEY UP */
			if(keyup != 0)
			{
				if(keyup & KEY_NAME_LEFT)
				{
					led_set_status(LED_NAME_LED1, LED_STATUS_OFF);
				}
				if(keyup & KEY_NAME_DOWN)
				{
					led_set_status(LED_NAME_LED2, LED_STATUS_OFF);
				}
				if(keyup & KEY_NAME_RIGHT)
				{
					led_set_status(LED_NAME_LED3, LED_STATUS_OFF);
				}
				if(keyup & KEY_NAME_BACK)
				{
					led_set_status(LED_NAME_LED4, LED_STATUS_OFF);
				}
			}

			/* KEY DOWN */
			if(keydown != 0)
			{
				if(keydown & KEY_NAME_LEFT)
				{
					led_set_status(LED_NAME_LED1, LED_STATUS_ON);
				}
				if(keydown & KEY_NAME_DOWN)
				{
					led_set_status(LED_NAME_LED2, LED_STATUS_ON);
				}
				if(keydown & KEY_NAME_RIGHT)
				{
					led_set_status(LED_NAME_LED3, LED_STATUS_ON);
				}
				if(keydown & KEY_NAME_BACK)
				{
					led_set_status(LED_NAME_LED4, LED_STATUS_ON);
				}
			}
		}
	}

	return 0;
}
