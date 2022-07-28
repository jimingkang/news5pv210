CC		= /arm/arm-2009q3/bin/arm-none-linux-gnueabi-gcc
LD 		= /arm/arm-2009q3/bin/arm-none-linux-gnueabi-ld
OBJCOPY	= /arm/arm-2009q3/bin/arm-none-linux-gnueabi-objcopy
OBJDUMP	= /arm/arm-2009q3/bin/arm-none-linux-gnueabi-objdump
AR		= /arm/arm-2009q3/bin/arm-none-linux-gnueabi-ar

INCDIR	:= $(shell pwd)
CPPFLAGS	:= -nostdlib -nostdinc -I$(INCDIR)/include
CFLAGS		:= -Wall -O2 -fno-builtin

#导出这些变量到全局，其实就是给子文件夹下面的Makefile使用
export CC LD OBJCOPY OBJDUMP AR CPPFLAGS CFLAGS


objs :=start.o uart.o buzzer.o _udivsi3.o _umodsi3.o clock.o  led.o main.o Hsmmc.o 

objs +=lib/libc.a

BL2.bin: $(objs) 
	arm-linux-ld -Tlink.lds -o BL2.elf $^
	arm-linux-objcopy -O binary BL2.elf BL2.bin
	arm-linux-objdump -D BL2.elf > BL2_elf.dis

lib/libc.a:
	cd lib;	make;	cd ..

	
%.o : %.S
	arm-linux-gcc  $(CPPFLAGS) $(CFLAGS) -o $@ $< -c -nostdlib

%.o : %.c
	arm-linux-gcc $(CPPFLAGS) $(CFLAGS) -o $@ $< -c -nostdlib

clean:
	rm *.o *.elf *.bin *.dis mkx210 -f

	
	