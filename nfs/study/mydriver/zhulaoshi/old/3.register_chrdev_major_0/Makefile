#ubuntu的内核源码树，如果要编译在ubuntu中安装的模块就打开这2个
#KERN_VER = $(shell uname -r)
#KERN_DIR = /lib/modules/$(KERN_VER)/build	

		
# 开发板的linux内核的源码树目录
KERN_DIR = /arm/x210ii_kernel

obj-m	+= module_test.o

all:
	make -C $(KERN_DIR) M=`pwd` modules 

cp:
	cp *.ko /arm-nfs/x210_driver

.PHONY: clean	
clean:
	make -C $(KERN_DIR) M=`pwd` modules clean
	

