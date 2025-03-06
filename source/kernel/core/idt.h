//
// Interrupt Descriptor Table
// Based on: https://wiki.osdev.org/Interrupt_Descriptor_Table
//

#pragma once

#include <stdint.h>

typedef enum
{
    IDT_FLAG_GATE_TASK              = 0x5,
    IDT_FLAG_GATE_16BIT_INT         = 0x6,
    IDT_FLAG_GATE_16BIT_TRAP        = 0x7,
    IDT_FLAG_GATE_32BIT_INT         = 0xE,
    IDT_FLAG_GATE_32BIT_TRAP        = 0xF,

    IDT_FLAG_RING0                  = (0 << 5),
    IDT_FLAG_RING1                  = (1 << 5),
    IDT_FLAG_RING2                  = (2 << 5),
    IDT_FLAG_RING3                  = (3 << 5),

    IDT_FLAG_PRESENT                = 0x80,

} IDT_FLAGS; // source: https://github.com/nanobyte-dev/nanobyte_os/blob/videos/part8/src/kernel/arch/i686/idt.h

void enable_interrupts(void);
void disable_interrupts(void);
void send_eoi(int irq);
void init_idt(void);
void init_pic(void);
void enable_irq(uint8_t irq);
void register_interrupt_handler(uint8_t interrupt_number, uint32_t handler_address);
