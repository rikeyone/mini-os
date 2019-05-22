ifneq ($(KERNELRELEASE),)
obj-m = pl2303.o
else
PWD = $(shell pwd)
K_DIR = ${PWD}/../../../kernel_obj

all:
	    make -C $(K_DIR) M=$(PWD) modules ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu-
clean:
	    make -C $(K_DIR) M=$(PWD) clean
endif
