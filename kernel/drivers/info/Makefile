
#Delete obj file first

#OBJFILE=$(objtree)/fs/proc/info/config_info.o
#$(shell rm $(OBJFILE) -f)


#FORCE will make sure this obj be compiled every time
ifneq ($(KERNELRELEASE),)
obj-m += config.o
config-objs = config_info.o
else
PWD = $(shell pwd)
K_DIR = ${PWD}/../../../kernel_obj
all:
	sh mkconfiginfo ${K_DIR} config_info.h
#	make -C $(K_DIR) M=$(PWD) modules ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu-
	make -C $(K_DIR) M=$(PWD) modules ARCH=arm CROSS_COMPILE=arm-linux-gnueabi-
clean:
	make -C $(K_DIR) M=$(PWD) clean
	$(shell rm config_info.h -f)
endif
