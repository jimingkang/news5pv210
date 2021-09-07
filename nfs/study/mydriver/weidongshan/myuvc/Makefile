#KERN_DIR = /usr/src/linux-headers-2.6.31-14-generic
# 开发板的linux内核的源码树目录
KERN_DIR = ~/news5pv210/x210ii_kernel
all:
	make -C $(KERN_DIR) M=`pwd` modules 

clean:
	make -C $(KERN_DIR) M=`pwd` modules clean
	rm -rf modules.order

obj-m	+= myuvc.o
