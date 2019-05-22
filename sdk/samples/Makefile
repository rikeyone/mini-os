PWD=$(shell pwd)
ROOTFSBIN=$(PWD)/../../rootfs/arm64/_install/bin

CC=aarch64-linux-gnu-gcc
STRIP=aarch64-linux-gnu-strip
CFLAGS= -Wall -Os -I../include 
LDFLAGS= -lpthread -lrt -lmini-ipc -L../lib

SRC=$(wildcard *.c)
TARGET=$(patsubst %.c,%,$(SRC))

.PHONY: clean all

all: $(TARGET)

$(TARGET): %:%.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	$(STRIP) $@
	mkdir -p $(ROOTFSBIN)
	cp $@ $(ROOTFSBIN)

clean:
	rm -f *.o $(TARGET)
