#ubuntu的内核源码树，如果要编译在ubuntu中安装的模块就打开这2个
#KERN_VER = $(shell uname -r)
#KERN_DIR = /lib/modules/$(KERN_VER)/build	

		
# 开发板的linux内核的源码树目录
KERN_DIR = ~/news5pv210/x210ii_kernel

obj-m	+= char_drv.o
all:
	make -C $(KERN_DIR) M=`pwd` modules 
	arm-none-linux-gnueabi-gcc  -static -o read read.c
	arm-none-linux-gnueabi-gcc  -static -o write write.c
	arm-none-linux-gnueabi-gcc  -static -o read_noblk read_noblk.c
	arm-none-linux-gnueabi-gcc  -static -o write_noblk write_noblk.c
cp:
	cp *.ko read write read_noblk write_noblk /tftpboot/nfs/mydriver_test

.PHONY: clean	
clean:
	make -C $(KERN_DIR) M=`pwd` modules clean
	rm read write read_noblk write_noblk
	

