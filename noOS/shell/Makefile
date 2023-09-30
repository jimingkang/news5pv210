CC		= arm-linux-gcc
LD 		= arm-linux-ld
OBJCOPY	= arm-linux-objcopy
OBJDUMP	= arm-linux-objdump
AR		= arm-linux-ar

INCDIR	:= $(shell pwd)
# C预处理器的flag，flag就是编译器可选的选项
CPPFLAGS	:= -nostdlib -nostdinc -I$(INCDIR)/include
# C编译器的flag
CFLAGS		:= -Wall -O2 -fno-builtin

#导出这些变量到全局，其实就是给子文件夹下面的Makefile使用
export CC LD OBJCOPY OBJDUMP AR CPPFLAGS CFLAGS


objs := start.o  clock.o  main.o sdram_init.o  sd_relocate.o   net/net.o dev/dev.o

#objs := start.o  clock.o uart.o main.o   irq.o myboot_i2c.o myboot_arp.o myboot_tftp.o myboot_dm9000.o myboot_key.o myboot_led.o
objs += lib/libc.a
objs += dev/dev.o
mynoos.bin: $(objs)
	$(LD) -Tlink.lds -o mynoos.elf $^
	$(OBJCOPY) -O binary mynoos.elf mynoos.bin
	$(OBJDUMP) -D mynoos.elf > mynoos_elf.dis
	gcc mkv210_image.c -o mkx210
	./mkx210 mynoos.bin 210.bin


	
%.o : %.S
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< -c

%.o : %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< -c
lib/libc.a:
	cd lib;	make;	cd ..
dev/dev.o :
	cd dev;	make;	cd ..
net/net.o :
	make -C net all



clean:
	rm *.o *.elf *.bin *.dis mkx210 -f
	make -C lib clean
	make -C dev clean
	make -C net clean


	
	
