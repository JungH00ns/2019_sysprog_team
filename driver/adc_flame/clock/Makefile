KERNEL_VER := 4.19.75-v7+
KERNEL_DIR = /lib/modules/$(KERNEL_VER)/build

obj-m := clock_dev.o 

PWD := $(shell pwd) 

all : 
	make -C $(KERNEL_DIR) M=$(PWD) modules
	sudo insmod clock_dev.ko
clean : 
	make -C $(KERNEL_DIR) M=$(PWD) clean
	sudo rmmod clock_dev

