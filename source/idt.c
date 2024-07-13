//
// Interrupt Descriptor Table - Created by Alan on 11/07/2024.
// Based on: https://wiki.osdev.org/Interrupt_Descriptor_Table
//

#include "idt.h"
#include "display.h"
#include "kernel.h"

struct InterruptDescriptor32 IDT[256];

void init_idt(void) {
    extern int load_idt(uint32_t*);
    extern int irq1();

    println("[IDT] Initializing IDT...");

    uint32_t irq1_address = (uint32_t)irq1;
    IDT[33].offset_lowerbits = irq1_address & 0xffff;
    IDT[33].selector = 0x08;
    IDT[33].zero = 0;
    IDT[33].type_attributes = 0x8e;
    IDT[33].offset_higherbits = (irq1_address >> 16) & 0xffff;

    uint32_t idt_address = (uint32_t)IDT;
    uint32_t idt_ptr[2];
    uint16_t idt_limit = sizeof(struct InterruptDescriptor32) * 256 - 1;
    idt_ptr[0] = idt_limit | ((idt_address & 0xFFFF) << 16);
    idt_ptr[1] = idt_address >> 16;

    /*
    print("[IDT] IDT address: ");
    print_uint32(idt_ptr[0]);
    println("");
     */

    load_idt(idt_ptr);

    println("[IDT] Initialized IDT");
}