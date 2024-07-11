nasm -f bin boot.asm -o boot.bin  # Assemble the bootloader

# Compile the kernel and link
i386-elf-gcc -ffreestanding -c kernel.c -o kernel.o
i386-elf-ld -T linker.ld -o kernel.bin --oformat binary kernel.o

cat boot.bin kernel.bin > os-image.bin

dd if=/dev/zero bs=512 count=2880 of=os-image.bin
dd if=boot.bin of=os-image.bin conv=notrunc
dd if=kernel.bin of=os-image.bin seek=1 conv=notrunc