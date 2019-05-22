#!/bin/bash

arch=$1

if [ "${arch}" = "arm" ];then
	killall qemu-system-arm 
else
	killall qemu-system-aarch64 
fi
