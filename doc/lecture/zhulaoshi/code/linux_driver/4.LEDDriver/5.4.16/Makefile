#ubuntu的内核源码树，如果要编译在ubuntu中安装的模块就打开这2个
#KERN_VER = $(shell uname -r)
#KERN_DIR = /lib/modules/$(KERN_VER)/build	

		
# 开发板的linux内核的源码树目录
#KERN_DIR = /root/driver/kernel
KERN_DIR = /home/jimmy/news5pv210/x210ii_kernel

obj-m	+= leds-s5pv210.o

all:
	make -C $(KERN_DIR) M=`pwd` modules 
	arm-none-linux-gnueabi-gcc app.c -o app

cp:
	sudo cp *.ko /tftpboot/nfs/driver_test
	sudo cp app /tftpboot/nfs/driver_test
	

.PHONY: clean	
clean:
	make -C $(KERN_DIR) M=`pwd` modules clean
	rm -rf app

