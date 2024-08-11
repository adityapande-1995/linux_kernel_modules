KERNEL_SRC := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
BUILD_DIR := $(PWD)/build

# Kernel module
obj-m := packet_sniffer.o

# Targets
all: build_dir sniffer_display packet_sniffer.ko

packet_sniffer.ko:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) modules

sniffer_display: sniffer_display.c
	mkdir -p $(BUILD_DIR)
	gcc -o $(BUILD_DIR)/sniffer_display sniffer_display.c

build_dir:
	mkdir -p $(BUILD_DIR)

clean:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) clean
	rm -rf $(BUILD_DIR)

