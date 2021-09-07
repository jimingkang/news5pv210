#include "stdio.h"
#include "int.h"
#include "main.h"


int main(void)
{
	uart_init();
	

	system_init_exception();
	rtc_set_alarm();
	
	intc_setvectaddr(NUM_RTC_ALARM, isr_rtc_alarm);
	intc_enable(NUM_RTC_ALARM);
	
	struct rtc_time tRead;
	
	while (1)
	{
		rtc_get_time(&tRead);
		printf("The time read is: %d.", tRead.second);
		
		volatile int i, j;
		for (i=0; i<10000; i++)
			for (j=0; j<1000; j++);
		printf("-------");
	}

/*
	printf("---rtc write time test---");
	struct rtc_time tWrite = 
	{
		.year = 2015,
		.month = 8,
		.date = 9,
		.hour = 18,
		.minute = 11,
		.second = 3,
		.day = 0,
	};
	rtc_set_time(&tWrite);
	
	
	printf("---rtc read time test---");
	struct rtc_time tRead;
	while (1)
	{
		rtc_get_time(&tRead);
		printf("The time read is: %d:%d:%d:%d:%d:%d:%d.", tRead.year, tRead.month, tRead.date, tRead.hour, tRead.minute, tRead.second, tRead.day);
		
		// 读写之间做点延时
		volatile int i, j;
		for (i=0; i<10000; i++)
			for (j=0; j<1000; j++);
		printf("-------");
	}
	
*/	
	
	
	while (1);

	return 0;
}