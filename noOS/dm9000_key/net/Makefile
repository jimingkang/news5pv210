objs := arp.o tftp.o


all : $(objs)
	/arm/arm-2009q3/bin/arm-linux-ld -r -o net.o $^
	
	
%.o:%.c
	${CC} $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

%.o:%.S
	${CC} $(CPPFLAGS) $(CFLAGS) -c -o $@ $<
	
clean :
	rm -f  *.o 
