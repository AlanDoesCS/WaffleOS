#!/bin/bash

qemu-system-i386 -hda ../builds/bin/os-image.bin -d int,cpu_reset -D ../builds/qemu.log -no-reboot
