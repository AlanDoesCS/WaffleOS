#!/bin/bash

# Create builds directory if it doesn't exist
mkdir -p ../builds/bin

# Assemble bootloader
nasm -f bin boot.asm -o ../builds/boot.bin
nasm -f elf32 interrupt.asm -o ../builds/interrupt.o
nasm -f elf32 io_functions.asm -o ../builds/io_functions.o

# Compile C files
i386-elf-gcc -ffreestanding -nostdlib -c kernel.c -o ../builds/kernel.o -m32
i386-elf-gcc -ffreestanding -nostdlib -c timer.c -o ../builds/timer.o -m32
i386-elf-gcc -ffreestanding -nostdlib -c disk.c -o ../builds/disk.o -m32
i386-elf-gcc -ffreestanding -nostdlib -c memory.c -o ../builds/memory.o -m32
i386-elf-gcc -ffreestanding -nostdlib -c display.c -o ../builds/display.o -m32
i386-elf-gcc -ffreestanding -nostdlib -c keyboard.c -o ../builds/keyboard.o -m32
i386-elf-gcc -ffreestanding -nostdlib -c idt.c -o ../builds/idt.o -m32
i386-elf-gcc -ffreestanding -nostdlib -c filesystem.c -o ../builds/filesystem.o -m32
i386-elf-gcc -ffreestanding -nostdlib -c str.c -o ../builds/str.o -m32

# Link the kernel
i386-elf-ld -o ../builds/kernel.bin -Tlinker.ld ../builds/kernel.o ../builds/disk.o ../builds/memory.o ../builds/display.o ../builds/timer.o ../builds/keyboard.o ../builds/interrupt.o ../builds/filesystem.o ../builds/idt.o ../builds/io_functions.o ../builds/str.o --oformat binary

dd if=/dev/zero of=../builds/bin/os-image.bin bs=1M count=4 # 4MB

# Create filesystem
mkfs.vfat -F 12 ../builds/bin/os-image.bin
# mkfs.vfat -F 16 ../builds/bin/os-image.bin  # FAT16
# mkfs.vfat -F 32 ../builds/bin/os-image.bin  # FAT32

# Mount the image
sudo mkdir -p /mnt/floppy
sudo mount ../builds/bin/os-image.bin /mnt/floppy

# Copy kernel to the mounted filesystem
sudo cp ../builds/kernel.bin /mnt/floppy/

# Unmount the image
sudo umount /mnt/floppy

# Write bootloader to first sector
dd if=../builds/boot.bin of=../builds/bin/os-image.bin conv=notrunc bs=512 count=1

# Run QEMU
qemu-system-i386 -drive format=raw,file=../builds/bin/os-image.bin,index=0,if=floppy -d int,cpu_reset -D ../builds/qemu.log # -no-reboot