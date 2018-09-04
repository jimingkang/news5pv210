#include <main.h>

int tester_asm_c(int argc, char * argv[])
{
	/*
	 * Write topic information
	 */
	s5pv210_serial_write_string(0, "transmit parameter from asm to c test...\r\n");

	serial_printf(0, "argc = %d\r\n", argc);
	while(1);

	return 0;
}
