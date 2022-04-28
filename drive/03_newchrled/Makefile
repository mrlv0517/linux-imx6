KERNELDIR := ~/linux/kernel
CURRNET_PATH := $(shell pwd)
obj-m := newchrled.o

build: kernel_modules

kernel_modules:
	$(MAKE) -C $(KERNELDIR) M=$(CURRNET_PATH) modules ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -j16

clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURRNET_PATH) clean


#build cmd : make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -j16
