all:
	gcc test.c -o test -laston -L.

lib: 
	gcc -o aston.o aston.c -c
	ar -rc libaston.a aston.o

clean:
	rm test
cleanall:
	rm test
	rm *.o *.a