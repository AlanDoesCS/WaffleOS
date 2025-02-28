# PROJECT MAKEFILE
include config.mk

.PHONY: all floppy_image kernel bootloader clean bochs lsimgroot hexdumpsector always

all: floppy_image

#
# Floppy image
#
floppy_image: $(BUILD_DIR)/main_floppy.img

$(BUILD_DIR)/main_floppy.img: bootloader kernel
	@dd if=/dev/zero of=$@ bs=512 count=2880 >/dev/null
	@mkfs.fat -F 12 -n "WFOS" $@ >/dev/null
	@dd if=$(BUILD_DIR)/stage1.bin of=$@ conv=notrunc >/dev/null
	@mcopy -i $@ $(BUILD_DIR)/stage2.bin "::stage2.bin"
	@mcopy -i $@ $(BUILD_DIR)/kernel.bin "::kernel.bin"
	@mcopy -i $@ test.txt "::test.txt"
	@mmd -i $@ "::mydir"
	@mcopy -i $@ test.txt "::mydir/test.txt"
	@echo "--> Created: " $@

#
# Bootloader
#
bootloader: stage1 stage2

stage1: $(BUILD_DIR)/stage1.bin

$(BUILD_DIR)/stage1.bin: always
	@$(MAKE) -C source/boot/stage1 BUILD_DIR=$(abspath $(BUILD_DIR))
	@echo "--> Created stage1 binary: $(BUILD_DIR)/stage1.bin"

stage2: $(BUILD_DIR)/stage2.bin

$(BUILD_DIR)/stage2.bin: always
	@$(MAKE) -C source/boot/stage2 BUILD_DIR=$(abspath $(BUILD_DIR))
	@echo "--> Created stage2 binary: $(BUILD_DIR)/stage2.bin"

#
# Kernel
#
kernel: $(BUILD_DIR)/kernel.bin

$(BUILD_DIR)/kernel.bin: always
	@$(MAKE) -C source/kernel BUILD_DIR=$(abspath $(BUILD_DIR))
	@echo "--> Created kernel binary: $(BUILD_DIR)/kernel.bin"

#
# Always
#
always:
	@mkdir -p $(BUILD_DIR)


#
# Run with Bochs
#
bochs:
	@bochs -f .bochsrc -q

lsimgroot:
	mdir -i $(BUILD_DIR)/main_floppy.img

hexdumpsector:
	hexdump -C -n 512 $(BUILD_DIR)/main_floppy.img

#
# Clean
#
clean:
	@$(MAKE) -C source/boot/stage1 BUILD_DIR=$(abspath $(BUILD_DIR)) clean
	@$(MAKE) -C source/boot/stage2 BUILD_DIR=$(abspath $(BUILD_DIR)) clean
	@$(MAKE) -C source/kernel BUILD_DIR=$(abspath $(BUILD_DIR)) clean
	@rm -rf $(BUILD_DIR)/*
