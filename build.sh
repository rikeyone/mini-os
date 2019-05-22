#!/bin/bash
# Desc:  MiniOS Main Build script !
# File:	    build.sh
# Author:	rikeyone
# Email:	xiehaocheng127@163.com
# Version:  V1.0

function usage()
{
	echo "MiniOS Build Script V1.0"
	echo "Usage: build.sh [OPTIONS]"
	echo "OPTIONS:"
	echo -e "\t-a         build all necessary modules"
	echo -e "\t-m module  just build one target module"
	echo -e "\t-h         show this tips"
	echo "Example:"
	echo "	./build.sh -a"
	echo "	./build.sh -m kernel"
	exit 1
}

if [ $# -lt 1 ];then
	usage
fi

echo "MiniOS Build Start..."

while getopts "cam:h" opt;
do
    case $opt in
        a)
		MOD=all
		;;
        m)
		MOD=${OPTARG}
		;;
        c)
		make -C external/busybox-1.24.0 mrproper
		rm -rf kernel_obj
		exit;;
        h)
		usage
		exit;;
        ?)
		echo "please use -h for help."
		exit;;
    esac
done

source scripts/envsetup.sh .

if [ x${MOD} = x ];then
	echo "Module can't be EMPTY!"
	exit 1
elif [ ${MOD} = "all" ];then
	echo "Build MiniOS."
	./scripts/build_kernel.sh arm64
	./scripts/build_rootfs.sh arm64
else
	SCRIPT=scripts/build_${MOD}.sh
	if [ ! -e ${SCRIPT} ];then
		echo "Module script:${SCRIPT} doesn't exsit!!!"
		exit 1
	fi
	echo "Build Target Module: ${MOD}"
	echo "Build Target SCRIPT: ${SCRIPT}"
	./${SCRIPT} arm64
fi

echo "Build Success!"
figlet -m smushmode minios
