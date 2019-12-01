KERNEL_DIR = /lib/modules/4.19.75-v7+/build

obj-m := ultra_dev.o

PWD := $(shell pwd)

all:
	make -C $(KERNEL_DIR) M=$(PWD) modules
	sudo insmod ultra_dev.ko
	
clean:
	make -C $(KERNEL_DIR) M=$(PWD) clean
	sudo rmmod ultra_dev.ko
