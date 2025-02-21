# Compiler and Assembler
CC = i386-elf-gcc
AS = nasm
LD = i386-elf-ld
CFLAGS = -ffreestanding -nostdlib -m32 # -g -Wall -Wextra
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
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(C_SOURCES)) \
          $(patsubst $(SRC_DIR)/%.asm, $(BUILD_DIR)/%.o, $(ASM_SOURCES))

# Special case: Bootloader
# Stage 1: boot.asm (must be 512 bytes)
BOOT1_ASM = $(SRC_DIR)/boot/boot.asm
BOOT1_BIN = $(BIN_DIR)/boot.bin
# Stage 2: boot2.asm (loaded as BOOT2.BIN from FAT12 filesystem)
BOOT2_ASM = $(SRC_DIR)/boot/boot2.asm
BOOT2_BIN = $(BIN_DIR)/boot2.bin
# Boot config: WFOSCFIG.asm (loaded as WFOSCFIG.BIN from filesystem)
BOOT_CFIG_ASM = $(SRC_DIR)/boot/WFOSCFIG.asm
BOOT_CFIG_BIN = $(BIN_DIR)/WFOSCFIG.bin

# Output
OS_IMAGE = $(BIN_DIR)/os-image.bin
KERNEL_BIN = $(BUILD_DIR)/kernel.bin

# Default target: Build all necessary directories and OS image.
all: $(BUILD_SUBDIRS) $(OS_IMAGE)

# Ensure all necessary directories exist
$(BUILD_SUBDIRS):
	mkdir -p $@

$(BIN_DIR):
	mkdir -p $@

# Assemble Stage 1 bootloader (raw binary)
$(BOOT1_BIN): $(BOOT1_ASM) | $(BIN_DIR)
	$(AS) -f bin $< -o $@

# Assemble Stage 2 bootloader (raw binary)
$(BOOT2_BIN): $(BOOT2_ASM) | $(BIN_DIR)
	$(AS) -f bin $< -o $@

# Assemble Boot config (raw binary)
$(BOOT_CFIG_BIN): $(BOOT_CFIG_ASM) | $(BIN_DIR)
	$(AS) -f bin $< -o $@

# Assemble other assembly files into .o
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.asm | $(BUILD_SUBDIRS)
	$(AS) -f elf32 $< -o $@

# Compile C files into .o
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_SUBDIRS)
	$(CC) $(CFLAGS) -c $< -o $@

# Link the kernel
$(KERNEL_BIN): $(OBJECTS) | $(BUILD_SUBDIRS)
	$(LD) -o $@ $(OBJECTS)

# Create a FAT12 OS image (1.44MB)
$(OS_IMAGE): $(BOOT1_BIN) $(BOOT2_BIN) $(BOOT_CFIG_BIN) $(KERNEL_BIN) | $(BIN_DIR)
	# Create a blank 1.44MB image
	dd if=/dev/zero of=$(OS_IMAGE) bs=512 count=2880
	mkfs.fat -F 12 -n "WAFFLE   OS" $(OS_IMAGE)
	dd if=$(BOOT1_BIN) of=$(OS_IMAGE) conv=notrunc
	# Copy files into the FAT12 filesystem using mtools
	mcopy -i $(OS_IMAGE) $(BOOT2_BIN) "::BOOT2.BIN"
	# mcopy -i $(OS_IMAGE) $(BOOT_CFIG_BIN) "::WFOSCFIG.BIN"
	mcopy -i $(OS_IMAGE) $(KERNEL_BIN) "::KERNEL.BIN"

lsimgmount:
	@echo "Mounting OS image and listing contents..."
	sudo mkdir -p $(MNT_DIR)
	# Force the filesystem type as vfat.
	sudo mount -o loop -t vfat $(OS_IMAGE) $(MNT_DIR)
	ls -l $(MNT_DIR)
	sudo umount $(MNT_DIR)

lsimgroot:
	mdir -i $(OS_IMAGE)

# Run with QEMU
run: $(OS_IMAGE)
	qemu-system-i386 -drive format=raw,file=$(OS_IMAGE),index=0,if=floppy -d int,cpu_reset -D $(BUILD_DIR)/qemu.log

# Run with Bochs for debugging
bochs: $(OS_IMAGE)
	bochs -f .bochsrc -q

# Debug with GDB
debug: $(OS_IMAGE)
	qemu-system-i386 -drive format=raw,file=$(OS_IMAGE),index=0,if=floppy -s -S &
	gdb -ex "target remote localhost:1234" -ex "symbol-file $(KERNEL_BIN)"

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)

hexdump:
	hexdump -C -n 64 $(OS_IMAGE)

hexdumpsector:
	hexdump -C -n 512 $(OS_IMAGE)
