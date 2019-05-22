PWD=$(shell pwd)
ROOTFSLIB=$(PWD)/../rootfs/arm64/_install/lib

.PHONY: all clean

all:
	mkdir -p $(ROOTFSLIB)
	cp lib/* $(ROOTFSLIB)
	make -C samples

clean:
	make -C samples clean	
