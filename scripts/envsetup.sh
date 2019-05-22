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

echo "ROOTDIR:${ROOTDIR}"
echo "envsetup...Done!"
