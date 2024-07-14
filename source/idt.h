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

void enable_interrupts(void);
void disable_interrupts(void);
void send_eoi(int irq);
void init_idt(void);
void init_pic(void);
void enable_irq(uint8_t irq);
void register_interrupt_handler(uint8_t interrupt_number, uint32_t handler_address);

#endif //IDT_H
