chmod +x build.sh
./build.sh    # Run build script first

qemu-system-i386 -drive format=raw,file=os-image.bin # Emulate in QEMU