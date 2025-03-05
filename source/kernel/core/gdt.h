// source: https://github.com/nanobyte-dev/nanobyte_os/blob/videos/part8/src/kernel/arch/i686/gdt.h
#pragma once

#define i686_GDT_CODE_SEGMENT 0x08
#define i686_GDT_DATA_SEGMENT 0x10

void i686_GDT_Initialize();
