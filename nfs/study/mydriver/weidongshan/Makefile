KERN_DIR = /work/jz2440/linux-3.4.2

all:
	make -C $(KERN_DIR) M=`pwd` modules 

clean:
	make -C $(KERN_DIR) M=`pwd` modules clean
	rm -rf modules.order

obj-m	+= cmos_ov7740_dev.o
obj-m	+= cmos_ov7740_drv.o
