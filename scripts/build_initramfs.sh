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

# generate cpio initramfs
cd rootfs/${arch}/_install
find . | cpio -H newc -ov --owner root:root > ../initramfs.cpio
cd ../
gzip initramfs.cpio

