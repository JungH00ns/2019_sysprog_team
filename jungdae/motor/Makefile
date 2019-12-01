KERNEL_VER := 4.19.75-v7+
KERNEL_DIR = /lib/modules/$(KERNEL_VER)/build

obj-m := motor_dev.o 

PWD := $(shell pwd) 

all : 
	make -C $(KERNEL_DIR) M=$(PWD) modules
	sudo insmod motor_dev.ko
clean : 
	make -C $(KERNEL_DIR) M=$(PWD) clean
	sudo rmmod motor_dev
