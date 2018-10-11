all1:
	gcc test.c -o test -laston -L.
	
all2:
	gcc test.c -o test -laston -L.
	

lib: 
	gcc -o aston.o aston.c -c
	ar -rc libaston.a aston.o
	
so:
	gcc -o aston.o aston.c -c -fPIC
	gcc -o libaston.so aston.o -shared -rpath.

clean:
	rm test
cleanall:
	rm test
	rm *.o *.a