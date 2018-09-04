#include <main.h>

int tester_serial_echo(int argc, char * argv[])
{
	u8_t buf = 0;

	/*
	 * Write topic information
	 */
	s5pv210_serial_write_string(0, "s5pv210 serial0 echo test, please type some character...\r\n");
	s5pv210_serial_write_string(1, "s5pv210 serial1 echo test, please type some character...\r\n");
	s5pv210_serial_write_string(2, "s5pv210 serial2 echo test, please type some character...\r\n");
	s5pv210_serial_write_string(3, "s5pv210 serial3 echo test, please type some character...\r\n");

	while(1)
	{
		if(s5pv210_serial_read(0, &buf, 1) == 1)
		{
			s5pv210_serial_write_string(0, "serial0 recv: [");
			s5pv210_serial_write(0, &buf, 1);
			s5pv210_serial_write_string(0, "]\r\n");
		}

		if(s5pv210_serial_read(1, &buf, 1) == 1)
		{
			s5pv210_serial_write_string(1, "serial1 recv: [");
			s5pv210_serial_write(1, &buf, 1);
			s5pv210_serial_write_string(1, "]\r\n");
		}

		if(s5pv210_serial_read(2, &buf, 1) == 1)
		{
			s5pv210_serial_write_string(2, "serial2 recv: [");
			s5pv210_serial_write(2, &buf, 1);
			s5pv210_serial_write_string(2, "]\r\n");
		}

		if(s5pv210_serial_read(3, &buf, 1) == 1)
		{
			s5pv210_serial_write_string(3, "serial3 recv: [");
			s5pv210_serial_write(3, &buf, 1);
			s5pv210_serial_write_string(3, "]\r\n");
		}
	}

	return 0;
}
