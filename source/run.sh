#!/bin/bash

chmod +x build.sh
./build.sh    # Run build script first

qemu-system-i386 -drive format=raw,file=../builds/bin/os-image.bin # Emulate in QEMU
# qemu-system-i386 -drive format=raw,file=os-image.bin -d int # Emulate in QEMU with debugging