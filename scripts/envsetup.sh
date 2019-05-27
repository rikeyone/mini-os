#!/bin/bash

echo "envsetup..."

if [ $# -lt 1 ];then
	echo "Failed:Please add an arg for \"ROOTDIR\" !"
	exit 1
fi

realpath .
CHECK_INSTALLED=$?

if [ ${CHECK_INSTALLED} != 0 ];then
    sudo apt-get install realpath
fi

REALDIR=$(realpath $1)
if [ x${REALDIR} = x ];then
	echo "Failed: \"ROOTDIR\" does not exsit !"
else
	export ROOTDIR=${REALDIR}
fi

export PATH=$PATH:/opt/gcc-linaro-4.9.4-2017.01-x86_64_aarch64-linux-gnu/bin
export PATH=$PATH:/opt/gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabi/bin

echo "ROOTDIR:${ROOTDIR}"
echo "envsetup...Done!"

