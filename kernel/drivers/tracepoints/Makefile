# builds the tracepoint example kernel modules;
# then to use one (as root):  insmod <module_name.ko>

ifneq ($(KERNELRELEASE),)
obj-m += tracepoint-sample.o
obj-m += tracepoint-probe-sample.o
obj-m += tracepoint-probe-sample2.o
else
PWD = $(shell pwd)
K_DIR = ${PWD}/../../../kernel_obj

all:
	make -C $(K_DIR) M=$(PWD) modules ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu-
#	make -C $(K_DIR) M=$(PWD) modules ARCH=arm CROSS_COMPILE=arm-linux-gnueabi-
clean:
	make -C $(K_DIR) M=$(PWD) clean
endif

