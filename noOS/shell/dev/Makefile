objs := led.o dm9000.o key.o int.o mmu.o uart.o   i2c.o lcd.o 

INCDIR	:= $(shell pwd)
CPPFLAGS	:= -nostdlib -nostdinc -I$(INCDIR)/../include

CFLAGS		:= -Wall -O2 -fno-builtin

all : $(objs)
	arm-linux-ld -r -o dev.o $^
	
	
%.o : %.c
	arm-linux-gcc ${CFLAGS} ${CPPFLAGS}  -c $^
	
%.o : %.S
	arm-linux-gcc ${CFLAGS} ${CPPFLAGS}  -c $^
	
clean :
	rm *.o  
