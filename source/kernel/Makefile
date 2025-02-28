# KERNEL MAKEFILE
# source: https://github.com/nanobyte-dev/nanobyte_os/blob/videos/part7/src/kernel/Makefile
BUILD_DIR ?= build
TARGET_CC ?= i686-elf-gcc
TARGET_ASM ?= nasm
TARGET_LD ?= i686-elf-ld

TARGET_ASMFLAGS += -f elf
TARGET_CFLAGS += -ffreestanding -nostdlib
TARGET_LIBS += -lgcc
TARGET_LINKFLAGS += -T linker.ld -nostdlib

SOURCES_C = $(wildcard *.c)
SOURCES_ASM = $(wildcard *.asm)
OBJECTS_C = $(patsubst %.c, $(BUILD_DIR)/kernel/c/%.obj, $(SOURCES_C))
OBJECTS_ASM = $(patsubst %.asm, $(BUILD_DIR)/kernel/asm/%.obj, $(SOURCES_ASM))

.PHONY: all kernel clean

all: kernel

kernel: $(BUILD_DIR)/kernel.bin

$(BUILD_DIR)/kernel.bin: $(OBJECTS_ASM) $(OBJECTS_C)
	@$(TARGET_LD) $(TARGET_LINKFLAGS) -Wl,-Map=$(BUILD_DIR)/kernel.map -o $@ $^ $(TARGET_LIBS)
	@echo "--> Created kernel.bin"

$(BUILD_DIR)/kernel/c/%.obj: %.c
	@mkdir -p $(@D)
	@$(TARGET_CC) $(TARGET_CFLAGS) -c -o $@ $<
	@echo "--> Compiled: $<"

$(BUILD_DIR)/kernel/asm/%.obj: %.asm
	@mkdir -p $(@D)
	@$(TARGET_ASM) $(TARGET_ASMFLAGS) -o $@ $<
	@echo "--> Compiled: $<"

clean:
	@rm -f $(BUILD_DIR)/kernel.bin $(BUILD_DIR)/kernel.map $(OBJECTS_C) $(OBJECTS_ASM)
