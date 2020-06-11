#!/bin/bash

sudo apt-get install qemu libncurses5-dev build-essential
#sudo apt-get install gcc-arm-linux-gnueabi
#sudo apt-get install gcc-aarch64-linux-gnu
sudo apt-get install figlet
sudo apt-get install bison flex

qemu-system-aarch64 -version

# install from source code if qemu-system-aarch64 not exsit (For version <= Ubuntu14.04)
if [ $? != 0 ]; then
	echo "qemu-system-aarch64 don't exsit! compile it!"
	sudo apt-get build-dep qemu
	wget https://download.qemu.org/qemu-2.11.0.tar.bz2
	tar -jxf qemu-2.11.0.tar.bz2
	cd qemu-2.11.0
	./configure --target-list=aarch64-softmmu
	make
	sudo make install
	rm qemu-2.11.0 -rf
else
	echo "qemu-system-aarch64 already exsit!"
fi

# install and use arm-linux-gnueabi version 4.9
if [ ! -e /opt/gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabi/ ]; then
	echo "install  arm-linux-gnueabi version 4.9 to /opt!"
	if [ ! -e gcc-toolchain/gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabi.tar.xz ]; then
		if [ ! -e gcc-toolchain ];then
			git clone -b arm-linux-gnueabi https://gitee.com/rikeyone/gcc-toolchain.git
		else
			cd gcc-toolchain
			git fetch origin arm-linux-gnueabi
			git checkout origin/arm-linux-gnueabi
			cd ..
		fi
	fi
	sudo tar -xvf gcc-toolchain/gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabi.tar.xz -C /opt/
else
	echo "arm-linux-gnueabi-gcc version 4.9 already installed to /opt!"
fi


# install and use aarch64-linux-gnu-gcc version 4.9
if [ ! -e /opt/gcc-linaro-4.9.4-2017.01-x86_64_aarch64-linux-gnu/ ]; then
	echo "install aarch64-linux-gnu-gcc version 4.9 to /opt!"
	if [ ! -e gcc-toolchain/gcc-linaro-4.9.4-2017.01-x86_64_aarch64-linux-gnu.tar.xz ]; then
		if [ ! -e gcc-toolchain ];then
			git clone -b aarch64-linux-gnu https://gitee.com/rikeyone/gcc-toolchain.git
		else
			cd gcc-toolchain
			git fetch origin aarch64-linux-gnu
			git checkout origin/aarch64-linux-gnu
			cd ..
		fi
	fi
	sudo tar -xvf gcc-toolchain/gcc-linaro-4.9.4-2017.01-x86_64_aarch64-linux-gnu.tar.xz -C /opt/
else
	echo "aarch64-linux-gnu-gcc version 4.9 already installed to /opt!"
fi

echo "Install success!!"
