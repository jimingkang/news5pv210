#include <main.h>

int tester_beep(int argc, char * argv[])
{
	while(1)
	{
		beep_set_status(BEEP_STATUS_ON);
		mdelay(500);

		beep_set_status(BEEP_STATUS_OFF);
		mdelay(500);
	}

	return 0;
}
