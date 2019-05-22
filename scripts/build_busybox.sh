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

echo root path is ${ROOTDIR}

if [ x${BUSYBOX} = x ];then
	BUSYBOX=busybox-1.24.0
fi

SRC=${ROOTDIR}/lib/busybox/${BUSYBOX}

if [ ! -e ${SRC} ];then
	cd ${ROOTDIR}/lib
	git clone -b ${BUSYBOX} https://github.com/rikeyone/busybox.git --depth=1
fi

cp ${ROOTDIR}/lib/minios-config/${BUSYBOX}/minios_arm64_defconfig ${ROOTDIR}/lib/busybox/${BUSYBOX}/configs/
cp ${ROOTDIR}/lib/minios-config/${BUSYBOX}/minios_arm32_defconfig ${ROOTDIR}/lib/busybox/${BUSYBOX}/configs/

build_arm64() {
	echo "start to build ARM64 $1 busybox!!"
	export ARCH=arm64
	export CROSS_COMPILE=aarch64-linux-gnu-
	make minios_arm64_defconfig
	make install
}

build_arm() {
	echo "start to build ARM $1 busybox!!"
	export ARCH=arm
	export CROSS_COMPILE=arm-linux-gnueabi-
	make minios_arm32_defconfig
	make install
}

#main entry

PLATFORM=$1
cd ${SRC}

case ${PLATFORM} in
	arm64)
		build_arm64 
		;;
	arm)
		build_arm 
		;;
	clean)
		echo "start to clean!!"
		make mrproper 
		;;
	*)
		echo "usage:"
		echo "./build.sh [platform]"
		echo " "
		echo "eg:"
		echo "   ./build.sh arm64     #build default  arm64 config"
		echo "   ./build.sh arm       #build default arm config"
		exit 1
		;;
esac


