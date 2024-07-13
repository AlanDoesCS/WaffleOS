//
// Interrupt Descriptor Table - Created by Alan on 11/07/2024.
// Based on: https://wiki.osdev.org/Interrupt_Descriptor_Table
//

#ifndef IDT_H
#define IDT_H

#include "types.h"

struct InterruptDescriptor32 {
    uint16_t offset_lowerbits;   // offset bits 0..15
    uint16_t selector;           // a code segment selector in GDT
    uint8_t zero;                    // unused, set to 0
    uint8_t type_attributes;         // gate type, dpl, and p fields
    uint16_t offset_higherbits;  // offset bits 16..31
} __attribute__((packed));

void init_idt(void);

#endif //IDT_H
