CC		= /arm/arm-2009q3/bin/arm-none-linux-gnueabi-gcc
LD 		= /arm/arm-2009q3/bin/arm-none-linux-gnueabi-ld
OBJCOPY	= /arm/arm-2009q3/bin/arm-none-linux-gnueabi-objcopy
OBJDUMP	= /arm/arm-2009q3/bin/arm-none-linux-gnueabi-objdump
AR		= /arm/arm-2009q3/bin/arm-none-linux-gnueabi-ar

INCDIR	:= $(shell pwd)
# C预处理器的flag，flag就是编译器可选的选项
CPPFLAGS	:= -nostdlib -nostdinc -I$(INCDIR)/include
# C编译器的flag
CFLAGS		:= -Wall -O2 -fno-builtin

#导出这些变量到全局，其实就是给子文件夹下面的Makefile使用
export CC LD OBJCOPY OBJDUMP AR CPPFLAGS CFLAGS


objs := start.o led.o clock.o uart.o main.o sdram_init.o buzzer.o  int.o key.o   sd_relocate.o  boot.o  lib/libc.a dev/dev.o net/net.o
#objs := start.o  clock.o uart.o main.o  mem_init.o irq.o myboot_i2c.o myboot_arp.o myboot_tftp.o myboot_dm9000.o myboot_key.o myboot_led.o


mynoos.bin: $(objs)
	$(LD) -Tlink.lds -o mynoos.elf $^
	$(OBJCOPY) -O binary mynoos.elf mynoos.bin
	$(OBJDUMP) -D mynoos.elf > mynoos_elf.dis
	gcc mkv210_image.c -o mkx210
	./mkx210 mynoos.bin 210.bin
	sudo cp mynoos.bin u-boot.bin
	sudo chmod 0777 u-boot.bin

lib/libc.a:
	cd lib;	make;	cd ..

dev/dev.o :
	cd dev;	make;	cd ..
	
net/net.o :
	cd net;	make;	cd ..
	
%.o : %.S
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< -c

%.o : %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< -c


clean:
	rm *.o *.elf *.bin *.dis mkx210 -f
	cd lib; make clean; cd ..
	cd dev; make clean; cd ..
	cd net; make clean; cd ..


	
	
