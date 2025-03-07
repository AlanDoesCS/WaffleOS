// Nanobyte's Hardware Abstraction Layer (HAL) implementation
// https://github.com/nanobyte-dev/nanobyte_os/blob/videos/part9/src/kernel/hal/hal.c
#include "hal.h"
#include "gdt.h"
#include "idt.h"
#include "isr.h"

void init_hal(void)
{
    i686_GDT_Initialize();
    init_idt();
    i686_ISR_Initialize();
}