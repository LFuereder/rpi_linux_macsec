obj-m += driver-test.o

KDIR = ../rootfs/lib/modules/6.6.25-v8-16k+/build

all:
	make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- -C $(KDIR) M=$(PWD) modules

.PHONY:clean new
clean:
	make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- -C $(KDIR) M=$(PWD) clean

new: clean all