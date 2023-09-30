
INCDIR	:= $(shell pwd)
CPPFLAGS	:= -nostdlib -nostdinc -I$(INCDIR)/../include

CFLAGS		:= -Wall -O2 -fno-builtin

objs := arp.o tftp.o


all : $(objs)
	arm-linux-ld -r -o net.o $^
	
	
%.o : %.c
	arm-linux-gcc  ${CPPFLAGS}  ${CFLAGS} -c $^
	
%.o : %.S
	arm-linux-gcc  ${CPPFLAGS} ${CFLAGS} -c $^
	
clean :
	rm *.o 
