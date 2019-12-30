#!/bin/bash

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

TARGET=$1
if [ ${TARGET} = "arm64" ];then
# extract aarch64 gcc toolchain
if [ -e /opt/gcc-linaro-4.9.4-2017.01-x86_64_aarch64-linux-gnu/ ]; then
	echo "aarch64-linux-gnu-gcc version 4.9 already installed to /opt!"
	SRC=/opt/gcc-linaro-4.9.4-2017.01-x86_64_aarch64-linux-gnu
else
	exit 1
fi

INSTALL=${ROOTDIR}/rootfs/arm64/_install

mkdir -p ${INSTALL}/lib
mkdir -p ${INSTALL}/usr/lib
cp -af ${SRC}/aarch64-linux-gnu/lib64/*.so* ${INSTALL}/lib
cp -af ${SRC}/aarch64-linux-gnu/libc/lib/*.so* ${INSTALL}/lib
cp -af ${SRC}/aarch64-linux-gnu/libc/usr/lib/*.so* ${INSTALL}/usr/lib
else
# extract arm gcc toolchain
if [ -e /opt/gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabi/ ]; then
	echo "arm-linux-gnueabi-gcc version 4.9 already installed to /opt!"
	SRC=/opt/gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabi
else
	exit 1
fi

INSTALL=${ROOTDIR}/rootfs/arm/_install

mkdir -p ${INSTALL}/lib
mkdir -p ${INSTALL}/usr/lib
cp -af ${SRC}/arm-linux-gnueabi/lib/*.so* ${INSTALL}/lib
cp -af ${SRC}/arm-linux-gnueabi/libc/lib/*.so* ${INSTALL}/lib
cp -af ${SRC}/arm-linux-gnueabi/libc/usr/lib/*.so* ${INSTALL}/usr/lib

fi

echo "Build libc success!!!"
