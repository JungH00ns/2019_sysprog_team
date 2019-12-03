KERNEL_DIR = /lib/modules/4.19.75-v7+/build

obj-m := vib_dev.o

PWD := $(shell pwd)

all:
	make -C $(KERNEL_DIR) M=$(PWD) modules
	sudo insmod vib_dev.ko
	
clean:
	make -C $(KERNEL_DIR) M=$(PWD) clean
	sudo rmmod vib_dev.ko
