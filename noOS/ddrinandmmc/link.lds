SECTIONS
{
	. = 0x20000000;
	
	.text : {
		start.o
		sdram_init.o
uart.o
buzzer.o
clock.o
main.o
		* (.text)
	}
    		
	.data : {
		* (.data)
	}
	
	bss_start = .; 
	.bss : {
		* (.bss)
	}
	
	bss_end  = .;	
}
