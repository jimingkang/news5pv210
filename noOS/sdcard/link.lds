SECTIONS
{
	. = 0x23E00000;
	
	.text : {
		start.o
		sdram_init.o
		clock.o
		uart.o
                                main.o
		Hsmmc.o
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
