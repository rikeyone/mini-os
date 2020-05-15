#!/bin/bash

arch=$1

if [ "${arch}" = "" ];then
	arch=arm64
fi

if [ x"${ROOTDIR}" = x ];then
    realpath .
    CHECK_INSTALLED=$?

    if [ ${CHECK_INSTALLED} != 0 ];then
        sudo apt-get install realpath
    fi

    REALDIR=$(realpath $0)
    SCRIPT_DIR=$(dirname ${REALDIR})
    ROOTDIR=$(dirname ${SCRIPT_DIR})
fi

echo root path is ${ROOTDIR}
cd ${ROOTDIR}

OBJ=kernel_obj

# generate cpio initramfs
cd rootfs/${arch}/_install
find . | cpio -H newc -ov --owner root:root > ../initramfs.cpio
cd ../
gzip initramfs.cpio

cd ${ROOTDIR}

if [ "${arch}" = "arm" ];then
	qemu-system-arm -M vexpress-a9 \
		-smp 4 \
		-m 1024M \
		-kernel ${OBJ}/arch/arm/boot/zImage \
		-append "rdinit=/linuxrc console=ttyAMA0 loglevel=8" \
		-dtb ${OBJ}/arch/arm/boot/dts/vexpress-v2p-ca9.dtb \
		-initrd rootfs/${arch}/initramfs.cpio.gz \
		-nographic
else
	qemu-system-aarch64 -machine virt \
		-qmp tcp:localhost:4444,server,nowait \
		-cpu cortex-a57 \
		-machine type=virt \
		-nographic -m 2048 \
		-smp 2 \
		-kernel ${OBJ}/arch/arm64/boot/Image \
		-append "rdinit=/linuxrc console=ttyAMA0 loglevel=8  trace_event=sched:*,timer:*,irq:* trace_buf_size=40M" \
		-netdev user,id=user0 \
		-device virtio-net-device,netdev=user0 \
		-initrd rootfs/${arch}/initramfs.cpio.gz
fi
