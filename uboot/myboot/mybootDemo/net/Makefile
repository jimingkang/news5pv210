objs := arp.o tftp.o


all : $(objs)
	arm-linux-ld -r -o net.o $^
	
	
%.o : %.c
	arm-linux-gcc ${CFLAGS} -c $^
	
%.o : %.S
	arm-linux-gcc ${CFLAGS} -c $^
	
clean :
	rm *.o 
