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

HOST_IP=192.168.1.1
TARGET_IP=192.168.1.101
NET_NUMBER=192.168.1.0
NET_MASK=255.255.255.0
NFS_ROOT=${ROOTDIR}/rootfs

sudo apt-get install net-tools
sudo apt-get install nfs-kernel-server
sudo apt-get install uml-utilities

if [ ! -e /etc/exports.bak ];then
	sudo cp /etc/exports /etc/exports.bak
fi

sudo echo "${NFS_ROOT}/${arch}/_install *(rw,sync,no_subtree_check,no_root_squash)" > /etc/exports
sudo /etc/init.d/nfs-kernel-server restart

TAP=$(ifconfig tap0 | head -n 1 | awk '{print $1}')

if [ x${TAP} = x ];then
	sudo tunctl -u $(whoami) -t tap0
	sudo ifconfig tap0 ${HOST_IP}
	sudo route add -net ${NET_NUMBER} netmask ${NET_MASK} dev tap0
	sudo sh -c "echo 1 > /proc/sys/net/ipv4/ip_forward"
fi

if [ "${arch}" = "arm" ];then
	qemu-system-arm -M vexpress-a9 \
		-smp 4 \
		-m 1024M \
		-kernel ${OBJ}/arch/arm/boot/zImage \
		-append "console=ttyAMA0 loglevel=8 root=/dev/nfs rw nfsroot=${HOST_IP}:${NFS_ROOT}/arm/_install,nolock ip=${TARGET_IP}" \
		-dtb ${OBJ}/arch/arm/boot/dts/vexpress-v2p-ca9.dtb \
		-net nic -net tap,ifname=tap0,script=no \
		-nographic
else
	qemu-system-aarch64 -machine virt \
		-cpu cortex-a57 \
		-machine type=virt \
		-nographic -m 2048 \
		-smp 2 \
		-kernel ${OBJ}/arch/arm64/boot/Image \
		-append "console=ttyAMA0 loglevel=8 root=/dev/nfs rw nfsroot=${HOST_IP}:${NFS_ROOT}/arm64/_install,nolock ip=${TARGET_IP}:${HOST_IP}:::::off::" \
		-netdev tap,id=tap0,ifname=tap0,script=no \
		-device virtio-net-device,netdev=tap0
fi
