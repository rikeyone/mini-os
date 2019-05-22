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
cd ${ROOTDIR}

PLATFORM=$1

if [ "$PLATFORM" = "arm" ]; then
	mkdir -p rootfs/arm/_install
	cd rootfs/arm/_install
else
	mkdir -p rootfs/arm64/_install
	cd rootfs/arm64/_install
fi


mkdir -p etc
mkdir -p dev
mkdir -p mnt
mkdir -p etc/init.d/

# create rcS for target board
cat > etc/init.d/rcS << EOF
mkdir -p /proc
mkdir -p /tmp
mkdir -p /sys
mkdir -p /mnt
/bin/mount -a
mkdir -p /dev/pts
mount -t devpts devpts /dev/pts
echo /sbin/mdev > /proc/sys/kernel/hotplug
mdev -s
EOF

chmod +x ./etc/init.d/rcS

# create fstab for target board
cat > etc/fstab << EOF
proc /proc proc defaults 0 0
tmpfs /tmp tmpfs defaults 0 0
sysfs /sys sysfs defaults 0 0
tmpfs /dev tmpfs defaults 0 0
debugfs /sys/kernel/debug debugfs defaults 0 0
EOF

# create inittab for target board
cat > etc/inittab << EOF
::sysinit:/etc/init.d/rcS
::respawn:-/bin/sh
::askfirst:-/bin/sh
::ctrlaltdel:/bin/umount -a -r
EOF

# create dev node

sudo mknod dev/console c 5 1
sudo mknod dev/null c 1 3

# build external library
# custom add @{
${ROOTDIR}/scripts/build_busybox.sh ${PLATFORM}
${ROOTDIR}/scripts/build_libc.sh ${PLATFORM}
#@}

echo "Build rootfs success!!"
