# STAGE 1 MAKEFILE
# source: https://github.com/nanobyte-dev/nanobyte_os/blob/videos/part7/src/bootloader/stage1/Makefile
BUILD_DIR ?= build
ASM ?= nasm

.PHONY: all clean

all: stage1

stage1: $(BUILD_DIR)/stage1.bin

$(BUILD_DIR)/stage1.bin: boot.asm
	@$(ASM) $< -f bin -o $@
	@echo "--> Created  stage1.bin"

clean:
	@rm -f $(BUILD_DIR)/stage1.bin
