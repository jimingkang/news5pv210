CC		= /arm/arm-2009q3/bin/arm-none-linux-gnueabi-gcc
LD 		= /arm/arm-2009q3/bin/arm-none-linux-gnueabi-ld
OBJCOPY	= /arm/arm-2009q3/bin/arm-none-linux-gnueabi-objcopy
OBJDUMP	= /arm/arm-2009q3/bin/arm-none-linux-gnueabi-objdump
AR		= /arm/arm-2009q3/bin/arm-none-linux-gnueabi-ar
INCDIR	:= $(shell pwd)
CPPFLAGS	:= -nostdlib -nostdinc -I$(INCDIR)/include

CFLAGS		:= -Wall -O2 -fno-builtin

led.bin: start.o 
	$(CC) $(CPPFLAGS) $(CFLAGS) -Ttext 0x0 -o led.elf $^
	$(OBJCOPY) -O binary led.elf led.bin
	$(OBJDUMP) -D led.elf > led_elf.dis
	gcc mkv210_image.c -o mkx210
	./mkx210 led.bin 210.bin
	
%.o : %.S
	$(CC)  $(CPPFLAGS) $(CFLAGS) -o $@ $< -c

%.o : %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< -c 

clean:
	rm *.o *.elf *.bin *.dis mkx210 -f

	
	