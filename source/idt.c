//
// Interrupt Descriptor Table - Created by Alan on 11/07/2024.
// Based on: https://wiki.osdev.org/Interrupt_Descriptor_Table
//

#include "idt.h"
#include "display.h"

struct InterruptDescriptor32 IDT[256];

void init_idt(void) {
    extern int load_idt(unsigned long*);
    extern int irq1();

    unsigned long irq1_address = (unsigned long)irq1;
    IDT[33].offset_lowerbits = irq1_address & 0xffff;
    IDT[33].selector = 0x08;
    IDT[33].zero = 0;
    IDT[33].type_attributes = 0x8e;
    IDT[33].offset_higherbits = (irq1_address & 0xffff0000) >> 16;

    unsigned long idt_address = (unsigned long)IDT;
    unsigned long idt_ptr[2];
    idt_ptr[0] = (sizeof(struct InterruptDescriptor32) * 256) + ((idt_address & 0xffff) << 16);
    idt_ptr[1] = idt_address >> 16;

    load_idt(idt_ptr);

    println("[IDT] Initialized IDT");
}