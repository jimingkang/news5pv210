all:
	arm-none-linux-gnueabi-gcc appfb.c -o appfb
cp:
	sudo cp -f  appfb /tftpboot/nfs/driver_test/ 
clean:
	rm appfb *.o
	rm -fr /tftpboot/nfs/driver_test/appfb 
