KERN_DIR =/home/jimmy/news5pv210/x210ii_kernel 

all:
	make -C $(KERN_DIR) M=`pwd` modules 

clean:
	make -C $(KERN_DIR) M=`pwd` modules clean
	rm -rf modules.order

obj-m	+= s3c_ts.o
