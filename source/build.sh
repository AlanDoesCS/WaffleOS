#!/bin/bash

# Create builds directory if it doesn't exist
mkdir -p ../builds/bin

# Assemble bootloader
nasm -f bin boot.asm -o ../builds/boot.bin
echo "Size of boot.bin: $(wc -c < ../builds/boot.bin) bytes"
nasm -f bin boot2.asm -o ../builds/boot2.bin
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
i386-elf-ld -o ../builds/kernel.bin -Ttext 0x20000 -Tlinker.ld ../builds/kernel.o ../builds/disk.o ../builds/memory.o ../builds/display.o ../builds/timer.o ../builds/keyboard.o ../builds/interrupt.o ../builds/filesystem.o ../builds/idt.o ../builds/io_functions.o ../builds/str.o --oformat binary

dd if=/dev/zero of=../builds/bin/os-image.bin bs=512 count=2880
mkfs.fat -F 12 -n "WAFFLE OS" ../builds/bin/os-image.bin

dd if=../builds/boot.bin of=../builds/bin/os-image.bin conv=notrunc bs=512 count=1

mkdir -p mnt
sudo mount ../builds/bin/os-image.bin mnt

# Copy the second stage bootloader and kernel to the disk image
sudo cp ../builds/boot2.bin mnt/BOOT2.BIN
sudo cp ../builds/kernel.bin mnt/KERNEL.BIN

# Unmount the disk image
sudo umount mnt
rmdir mnt

# hexdump
hexdump -C ../builds/bin/os-image.bin # | head -n 20

# Print partition table
# sudo parted ../builds/bin/os-image.bin print

# Run QEMU
# qemu-system-i386 -drive format=raw,file=../builds/bin/os-image.bin,index=0,if=ide -d int,cpu_reset -D ../builds/qemu.log
# floppy
qemu-system-i386 -drive file=../builds/bin/os-image.bin,format=raw,index=0,if=floppy -boot a -d int,cpu_reset -D ../builds/qemu.log -monitor stdio #-no-reboot
