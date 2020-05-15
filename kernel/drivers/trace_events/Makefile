# builds the trace events example kernel modules;
# then to use one (as root):  insmod <module_name.ko>

# If you include a trace header outside of include/trace/events
# then the file that does the #define CREATE_TRACE_POINTS must
# have that tracer file in its main search path. This is because
# define_trace.h will include it, and must be able to find it from
# the include/trace directory.
#
# Here trace-events-sample.c does the CREATE_TRACE_POINTS.
#

ifneq ($(KERNELRELEASE),)
CFLAGS_trace-events-sample.o := -I$(src)
obj-m += trace-events-sample.o
else
PWD = $(shell pwd)
K_DIR = ${PWD}/../../../kernel_obj

all:
	make -C $(K_DIR) M=$(PWD) modules ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu-
#   make -C $(K_DIR) M=$(PWD) modules ARCH=arm CROSS_COMPILE=arm-linux-gnueabi-
clean:
	make -C $(K_DIR) M=$(PWD) clean
endif


