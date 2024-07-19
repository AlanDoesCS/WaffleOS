#!/bin/bash

qemu-system-i386 -drive format=raw,file=../builds/bin/os-image.bin,index=0,if=floppy -d int,cpu_reset -D ../builds/qemu.log -no-reboot
