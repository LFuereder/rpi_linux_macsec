#modified LVB 27/6/2021 to make depmod a separate line item for install
SHELL = /bin/bash
ifneq ($(xvc_bar_num),)
	XVC_FLAGS += -D__XVC_BAR_NUM__=$(xvc_bar_num)
endif

ifneq ($(xvc_bar_offset),)
	XVC_FLAGS += -D__XVC_BAR_OFFSET__=$(xvc_bar_offset)
endif

$(warning XVC_FLAGS: $(XVC_FLAGS).)

topdir := $(shell cd $(src)/.. && pwd)

TARGET_MODULE:=xdma

EXTRA_CFLAGS := -I$(topdir)/include $(XVC_FLAGS)
#EXTRA_CFLAGS += -D__LIBXDMA_DEBUG__
#EXTRA_CFLAGS += -DINTERNAL_TESTING

ifneq ($(KERNELRELEASE),)
	$(TARGET_MODULE)-objs := libxdma.o xdma_cdev.o cdev_ctrl.o cdev_events.o cdev_sgdma.o cdev_xvc.o cdev_bypass.o xdma_mod.o xdma_thread.o
	obj-m := $(TARGET_MODULE).o
	obj-m += kritis3m_egress_kthread.o
	obj-m += macsec.o
	
else
	BUILDSYSTEM_DIR:=../../../rootfs/lib/modules/6.6.25-v8-16k+/build
	PWD:=$(shell pwd)
all :
	make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- -C $(BUILDSYSTEM_DIR) M=$(PWD) modules

clean:
	make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- -C $(BUILDSYSTEM_DIR) M=$(PWD) clean
	@/bin/rm -f *.ko modules.order *.mod.c *.o *.o.ur-safe .*.o.cmd

new: clean all


endif
