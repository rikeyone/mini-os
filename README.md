# Description

MiniOS is an develop environment for linux kernel and applications using QEMU virtual machine. Developers can easily clone from it and develop based on it. One can make his idea fast to an product using MiniOS environment. It is a good choice to verify your solutions on MiniOS before production.

# Install

```
sudo ./install.sh
```
It will install compile toolchains of arm and arm64, and qemu environment.

# Build MiniOS

To build the full MiniOS system:

```
sudo ./build.sh -a
```

To build MiniOS modules:

```
sudo ./build.sh -m {MODDULE}
```

the module has these values to choose: kernel, rootfs, busybox, libc, initramfs.
BTW, the rootfs module will include "libc", "initramfs" and "busybox" build.

eg:

```
sudo ./build.sh rootfs    #compile arm64 platform rootfs module
sudo ./build.sh kernel    #compile arm64 platform kernel module
```

# Run qemu

```
sudo ./run.sh
```

# Stop qemu

```
sudo ./run.sh -s
```
