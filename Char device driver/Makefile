obj-m = CharModule.o

KERNEL_DIR =$(shell uname -r)

all:
	make -C /lib/modules/$(KERNEL_DIR)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(KERNEL_DIR)/build M=$(PWD) clean
	
