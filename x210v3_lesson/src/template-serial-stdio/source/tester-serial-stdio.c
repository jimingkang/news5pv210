#include <main.h>

int tester_serial_stdio(int argc, char * argv[])
{
	int i_num = 0;
	double f_num = 3.1415926;

	/*
	 * Write topic information
	 */
	s5pv210_serial_write_string(2, "s5pv210 serial0 stdio test ...\r\n");

	while(1)
	{
		serial_printf(2, "i_num = %d, f_num = %f\r\n", i_num++, f_num += 0.000001);
		mdelay(200);
	}

	return 0;
}
