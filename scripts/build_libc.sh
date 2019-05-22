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

# extract aarch64 gcc toolchain
if [ ! -e /opt/gcc-linaro-4.9.4-2017.01-x86_64_aarch64-linux-gnu/ ]; then
	tar -xvf ${ROOTDIR}/toolchain/gcc-linaro-4.9.4-2017.01-x86_64_aarch64-linux-gnu.tar.xz -C ${ROOTDIR}/toolchain/
	SRC=${ROOTDIR}/toolchain/gcc-linaro-4.9.4-2017.01-x86_64_aarch64-linux-gnu
else
	echo "aarch64-linux-gnu-gcc version 4.9 already installed to /opt!"
	SRC=/opt/gcc-linaro-4.9.4-2017.01-x86_64_aarch64-linux-gnu
fi

TARGET=${ROOTDIR}/rootfs/arm64/_install

mkdir -p ${TARGET}/lib
mkdir -p ${TARGET}/usr/lib
cp -af ${SRC}/aarch64-linux-gnu/lib64/*.so* ${TARGET}/lib
cp -af ${SRC}/aarch64-linux-gnu/libc/lib/*.so* ${TARGET}/lib
cp -af ${SRC}/aarch64-linux-gnu/libc/usr/lib/*.so* ${TARGET}/usr/lib

echo "Build libc success!!!"
