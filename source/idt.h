//
// Interrupt Descriptor Table - Created by Alan on 11/07/2024.
// Based on: https://wiki.osdev.org/Interrupt_Descriptor_Table
//

#ifndef IDT_H
#define IDT_H

struct InterruptDescriptor32 {
    unsigned short int offset_lowerbits;   // offset bits 0..15
    unsigned short int selector;           // a code segment selector in GDT
    unsigned char zero;                    // unused, set to 0
    unsigned char type_attributes;         // gate type, dpl, and p fields
    unsigned short int offset_higherbits;  // offset bits 16..31
};

void init_idt(void);

#endif //IDT_H
