#!/bin/bash
# Desc:  MiniOS Run Script !
# File:	    run.sh
# Author:	rikeyone
# Email:	xiehaocheng127@163.com
# Version:  V1.0

function usage()
{
	echo "MiniOS Run Script V1.0"
	echo "Usage: ./run.sh [-t type][-d][-h][-s][-p plat]"
	echo "OPTIONS:"
	echo -e "\t-t type      choose a start rootfs type [nfs/initramfs]"
	echo -e "\t-p platform  choose a platform type [arm/arm64]"
	echo -e "\t-d         start using gdb option"
	echo -e "\t-s         shutdown ARM64 qemu process"
	echo -e "\t-h         show this tips"
	echo "Example:"
	echo "	./run.sh -t"
	echo "	./build.sh -m kernel"
	exit 1
}

if [ ! x"${USER}" = x"root" ];then
	echo "Please rerun `basename $0` as root!!!"
	exit 1
fi

echo ${USER}


while getopts "p:t:dsh" opt;
do
    case $opt in
    t)
		if [ ${OPTARG} = "nfs" ];then
			START=scripts/qemu_nfs_start.sh
		else
			START=scripts/qemu_start.sh
		fi
		;;
    d)
		START=scripts/qemu_gdb_start.sh
		;;
    s)
		START=scripts/qemu_stop.sh
		;;
    p)
		PLATFORM=${OPTARG}
		;;
    h)
		usage
		exit;;
    ?)
		echo "please use -h for help."
		exit;;
    esac
done

if [ x${START} = x ];then
	START=scripts/qemu_start.sh
fi

if [ x${PLATFORM} = x ];then
	PLATFORM=arm64
fi

./${START} ${PLATFORM}
