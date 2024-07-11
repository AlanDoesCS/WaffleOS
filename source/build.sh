#!/bin/bash

# Assemble bootloader
nasm -f bin boot.asm -o boot.bin
nasm -f elf32 interrupt.asm -o interrupt.o

i386-elf-gcc -ffreestanding -nostdlib -c kernel.c -o kernel.o -m32
i386-elf-gcc -ffreestanding -nostdlib -c display.c -o display.o -m32
i386-elf-gcc -ffreestanding -nostdlib -c keyboard.c -o keyboard.o -m32
i386-elf-gcc -ffreestanding -nostdlib -c idt.c -o idt.o -m32

# Link the kernel
i386-elf-ld -o kernel.bin -Tlinker.ld kernel.o display.o keyboard.o interrupt.o idt.o --oformat binary

cat boot.bin kernel.bin > os-image.bin

dd if=/dev/zero of=os-image.bin bs=512 count=2880
dd if=boot.bin of=os-image.bin conv=notrunc
dd if=kernel.bin of=os-image.bin seek=1 conv=notrunc