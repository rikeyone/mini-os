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
	echo -e "\t-p arm64   select platform, options: arm, arm64[default]"
	echo -e "\t-h         show this tips"
	echo "Example:"
	echo "	./build.sh -a"
	echo "	./build.sh -m kernel -p arm64"
	exit 1
}

if [ $# -lt 1 ];then
	usage
fi

echo "MiniOS Build Start..."

while getopts "cam:p:h" opt;
do
    case $opt in
        a)
		MOD=all
		;;
        m)
		MOD=${OPTARG}
		;;
        p)
		PLATFORM=${OPTARG}
		;;
        c)
		rm -rf kernel_obj
		rm -rf rootfs
		cd lib/busybox
		git clean -f -d
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

if [ x${PLATFORM} = x ];then
	PLATFORM=arm64
fi

if [ x${MOD} = x ];then
	echo "Module can't be EMPTY!"
	exit 1
elif [ ${MOD} = "all" ];then
	echo "Build MiniOS."
	./scripts/build_kernel.sh ${PLATFORM}
	./scripts/build_rootfs.sh ${PLATFORM}
else
	SCRIPT=scripts/build_${MOD}.sh
	if [ ! -e ${SCRIPT} ];then
		echo "Module script:${SCRIPT} doesn't exsit!!!"
		exit 1
	fi
	echo "Build Target Module: ${MOD}"
	echo "Build Target SCRIPT: ${SCRIPT}"
	./${SCRIPT} ${PLATFORM}
fi

echo "Build Success!"
figlet -m smushmode minios
