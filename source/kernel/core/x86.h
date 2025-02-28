// source: https://github.com/nanobyte-dev/nanobyte_os/blob/videos/part7/src/kernel/x86.h

#pragma once
#include <stdint.h>
#include <stdbool.h>

void __attribute__((cdecl)) x86_outb(uint16_t port, uint8_t value);
uint8_t __attribute__((cdecl)) x86_inb(uint16_t port);
