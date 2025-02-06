# Compiler and Assembler
CC = i386-elf-gcc
AS = nasm
LD = i386-elf-ld
CFLAGS = -ffreestanding -nostdlib -m32 -g -Wall -Wextra
LDFLAGS = -T linker.ld --oformat binary

# Directories
SRC_DIR = source
BUILD_DIR = builds
BIN_DIR = $(BUILD_DIR)/bin
MNT_DIR = /mnt/floppy

# Create list of subdirectories in `source/`
BUILD_SUBDIRS = $(patsubst $(SRC_DIR)/%, $(BUILD_DIR)/%, $(shell find $(SRC_DIR) -type d))

# Source Files
ASM_SOURCES = $(wildcard $(SRC_DIR)/core/*.asm $(SRC_DIR)/hardware/*.asm)
C_SOURCES = $(wildcard $(SRC_DIR)/**/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(C_SOURCES)) $(patsubst $(SRC_DIR)/%.asm, $(BUILD_DIR)/%.o, $(ASM_SOURCES))

# Special case: Bootloader
BOOT_ASM = $(SRC_DIR)/boot/boot.asm
BOOT_BIN = $(BIN_DIR)/boot.bin

# Output
OS_IMAGE = $(BIN_DIR)/os-image.bin
KERNEL_BIN = $(BUILD_DIR)/kernel.bin

# Default target
all: $(BUILD_SUBDIRS) $(OS_IMAGE)

# Ensure all necessary directories exist before building
$(BUILD_SUBDIRS):
	mkdir -p $@

# Assemble bootloader separately (raw binary)
$(BOOT_BIN): $(BOOT_ASM) | $(BIN_DIR)
	$(AS) -f bin $< -o $@

# Assemble other assembly files into .o
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.asm | $(BUILD_SUBDIRS)
	$(AS) -f elf32 $< -o $@

# Compile C files into .o
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_SUBDIRS)
	$(CC) $(CFLAGS) -c $< -o $@

# Rule for BIN_DIR
$(BIN_DIR):
	mkdir -p $@

# Link the kernel
$(KERNEL_BIN): $(OBJECTS) | $(BUILD_SUBDIRS)
	$(LD) -o $@ $(OBJECTS)

# Create an empty OS image (4MB)
$(OS_IMAGE): $(BOOT_BIN) $(KERNEL_BIN) | $(BIN_DIR)
	dd if=/dev/zero of=$@ bs=512 count=2880
	mkfs.vfat -F 12 $@
	sudo mkdir -p $(MNT_DIR)
	sudo mount $@ $(MNT_DIR)
	sudo cp $(KERNEL_BIN) $(MNT_DIR)/
	sudo umount $(MNT_DIR)
	dd if=$(BOOT_BIN) of=$@ conv=notrunc bs=512 count=1

# Run with QEMU
run: $(OS_IMAGE)
	qemu-system-i386 -drive format=raw,file=$(OS_IMAGE),index=0,if=floppy -d int,cpu_reset -D $(BUILD_DIR)/qemu.log

# Run with Bochs for better debugging
bochs: $(OS_IMAGE)
	bochs -f .bochsrc -q

# Debug with GDB
debug: $(OS_IMAGE)
	qemu-system-i386 -drive format=raw,file=$(OS_IMAGE),index=0,if=floppy -s -S &
	gdb -ex "target remote localhost:1234" -ex "symbol-file $(KERNEL_BIN)"

# Clean build artifacts (recursively remove all .o files)
clean:
	rm -rf $(BUILD_DIR)
