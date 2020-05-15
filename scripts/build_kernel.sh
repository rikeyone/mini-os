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

## Find the linux kernel version and download
if [ x"${LINUX_KERNEL}" = x ];then
	LINUX_KERNEL=linux-4.0
fi

echo root path is ${ROOTDIR}
echo linux version is ${LINUX_KERNEL}

SRC=${ROOTDIR}/kernel/linux-kernel/${LINUX_KERNEL}
OBJ=${ROOTDIR}/kernel_obj

if [ ! -e ${ROOTDIR}/kernel/linux-kernel ];then
	cd ${ROOTDIR}/kernel
	git clone -b ${LINUX_KERNEL} https://gitee.com/rikeyone/linux-kernel.git --depth=1
elif [ ! -e ${SRC} ];then
	cd ${ROOTDIR}/kernel/linux-kernel
	git clean -f -d
	git fetch origin ${LINUX_KERNEL}:${LINUX_KERNEL}
	git checkout ${LINUX_KERNEL}
fi

build_arm64() {
	echo "start to build ARM64 $1 kernel image!!"
	export ARCH=arm64
	export CROSS_COMPILE=aarch64-linux-gnu-
	make $1 O=${OBJ}
	make O=${OBJ} -j8
}

build_arm() {
	echo "start to build ARM $1 kernel image!!"
	export ARCH=arm
	export CROSS_COMPILE=arm-linux-gnueabi-
	make $1 O=${OBJ}
	make bzImage O=${OBJ} -j8
	make dtbs O=${OBJ}
}

#main entry
PLATFORM=$1
if [ -e ${OBJ}/arch/arm64/boot/Image -a "${PLATFORM}" != "arm64" ]; then
	echo "arch/arm64/boot/Image exist, make distclean"
	rm -rf ${OBJ}
elif [ -e ${OBJ}/arch/arm/boot/zImage -a "${PLATFORM}" != "arm" ]; then
	echo "arch/arm/boot/zImage exist, make distclean"
	rm -rf ${OBJ}
fi

if [ ! -f ${ROOTDIR}/kernel/linux-kernel/${LINUX_KERNEL}/arch/arm/configs/minios_defconfig ];then
	cp ${ROOTDIR}/kernel/minios-config/${LINUX_KERNEL}/minios_arm32_defconfig \
		${ROOTDIR}/kernel/linux-kernel/${LINUX_KERNEL}/arch/arm/configs/minios_defconfig
fi

if [ ! -f ${ROOTDIR}/kernel/linux-kernel/${LINUX_KERNEL}/arch/arm64/configs/minios_defconfig ];then
	cp ${ROOTDIR}/kernel/minios-config/${LINUX_KERNEL}/minios_arm64_defconfig \
		${ROOTDIR}/kernel/linux-kernel/${LINUX_KERNEL}/arch/arm64/configs/minios_defconfig
fi

cd ${SRC}
case ${PLATFORM} in
	arm64)
		build_arm64 minios_defconfig
		;;
	arm)
		build_arm minios_defconfig
		;;
	select)
		echo "start to select config to build kernel image!!"
		i=0
		arch=($(ls arch/))
		for file in ${arch[@]}
		do
			echo ${i}:${file##*/};
			((i++));
		done
		echo please input your choice:
		read index
		target_arch=${arch[${index}]##*/}

		i=0
		configs=($(ls arch/${target_arch}/configs/*))
		for file in ${configs[@]}
		do
			echo ${i}:${file##*/};
			((i++));
		done

		echo please input your choice:
		read index
		target_config=${configs[${index}]##*/}
		echo target is: ${target_arch} ${target_config}

		if [ ${target_arch} = "arm64" ];then
			build_arm64 ${target_config}
		else
			build_arm ${target_config}
		fi
		;;
	clean)
		echo "start to clean!!"
		rm arch/arm64/boot/Image -f
		rm arch/arm/boot/zImage -f
		make distclean
		;;
	*)
		echo "usage:"
		echo "./build.sh [platform]"
		echo " "
		echo "eg:"
		echo "   ./build.sh arm64     #build default  arm64 config"
		echo "   ./build.sh arm       #build default arm config"
		echo "   ./build.sh select    #select platform and config to build"
		;;
esac

