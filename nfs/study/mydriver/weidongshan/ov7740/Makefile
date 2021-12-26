KERN_DIR= ~/news5pv210/x210ii_kernel

all:
	make -C $(KERN_DIR) M=`pwd` modules 

clean:
	make -C $(KERN_DIR) M=`pwd` modules clean
	rm -rf modules.order

obj-m	+= cmos_ov7740_dev.o
obj-m	+= cmos_ov7740_drv.o
