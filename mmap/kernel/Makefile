

#KERNEL_DIR := /second/fsl/android/kk4.4.2/kernel_imx
KERNEL_DIR := /second/fsl/kernel/linux-2.6-imx
ARCH_TYPE := arm
CROSS_COMPILE := /opt/freescale/usr/local/gcc-4.6.2-glibc-2.13-linaro-multilib-2011.12/fsl-linaro-toolchain/bin/arm-fsl-linux-gnueabi-
#CROSS_COMPILE := /second/fsl/android/kk4.4.2/prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/arm-eabi-

### ubuntu
#KERNEL_DIR = /lib/modules/$(shell uname -r)/build


obj-m += test.o

all:
	echo "build hello"
	make -C $(KERNEL_DIR) M=`pwd` ARCH=$(ARCH_TYPE) CROSS_COMPILE=$(CROSS_COMPILE) modules

clean:
	#rm modules.order Module.symvers hello.ko hello.mod.c hello.mod.o hello.o
	rm modules.order Module.symvers *.ko *.mod.c *.mod.o *.o
