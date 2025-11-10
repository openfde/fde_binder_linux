ccflags-y = -I$(src) -DCONFIG_ANDROID_BINDERFS -DCONFIG_ANDROID_BINDER_DEVICES_HACKED="\"binder,hwbinder,vndbinder\""
obj-m := binder_linux.o
binder_linux-y := binderfs.o binder.o binder_alloc.o idr.o deps.o


ifeq ($(KERNEL_SRC),)
  ifeq ($(KDIR),)
    KDIR := /lib/modules/$(shell uname -r)/build
  endif
else
  KDIR := $(KERNEL_SRC)
endif

all:
	$(MAKE) -C $(KDIR) M=$$PWD

install: all
	insmod binder_linux.ko

modules_install:
	$(MAKE) -C $(KDIR) M=$$PWD modules_install

uninstall:
	rmmod binder_linux

clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean
	rm *.o.*

