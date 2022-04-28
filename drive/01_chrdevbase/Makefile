KERNELDIR := ~/linux/kernel
CURRNET_PATH := $(shell pwd)
obj-m := chrdevbase.o

build: kernel_modules

kernel_modules:
	$(MAKE) -C $(KERNELDIR) M=$(CURRNET_PATH) modules

clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURRNET_PATH) clean


#build cmd : make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -j16