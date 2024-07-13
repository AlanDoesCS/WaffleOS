#!/bin/bash

chmod +x build.sh
./build.sh    # Run build script first

qemu-system-i386 -drive format=raw,file=../builds/bin/os-image.bin -d int,cpu_reset -D ../builds/qemu.log -no-reboot