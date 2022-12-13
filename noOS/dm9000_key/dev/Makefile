objs :=  dm9000.o 


all : $(objs)
	$(LD) -r -o dev.o $^
	
	
%.o:%.c
	${CC} $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

%.o:%.S
	${CC} $(CPPFLAGS) $(CFLAGS) -c -o $@ $<
	
clean :
	rm -f  *.o  
