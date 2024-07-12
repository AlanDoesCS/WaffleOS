#!/bin/bash

# Create builds directory if it doesn't exist
mkdir -p ../builds

# Assemble bootloader
nasm -f bin boot.asm -o ../builds/boot.bin
nasm -f elf32 interrupt.asm -o ../builds/interrupt.o
nasm -f elf32 io_functions.asm -o ../builds/io_functions.o

i386-elf-gcc -ffreestanding -nostdlib -c kernel.c -o ../builds/kernel.o -m32
i386-elf-gcc -ffreestanding -nostdlib -c display.c -o ../builds/display.o -m32
i386-elf-gcc -ffreestanding -nostdlib -c keyboard.c -o ../builds/keyboard.o -m32
i386-elf-gcc -ffreestanding -nostdlib -c idt.c -o ../builds/idt.o -m32
i386-elf-gcc -ffreestanding -nostdlib -c fat.c -o ../builds/fat.o -m32
i386-elf-gcc -ffreestanding -nostdlib -c str.c -o ../builds/str.o -m32

# Link the kernel
i386-elf-ld -o ../builds/kernel.bin -Tlinker.ld ../builds/kernel.o ../builds/display.o ../builds/keyboard.o ../builds/interrupt.o ../builds/fat.o ../builds/idt.o ../builds/io_functions.o ../builds/str.o --oformat binary

cat ../builds/boot.bin ../builds/kernel.bin > ../builds/bin/os-image.bin

dd if=/dev/zero of=../builds/bin/os-image.bin bs=512 count=2880
dd if=../builds/boot.bin of=../builds/bin/os-image.bin conv=notrunc
dd if=../builds/kernel.bin of=../builds/bin/os-image.bin seek=1 conv=notrunc