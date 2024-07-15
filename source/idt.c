//
// Interrupt Descriptor Table - Created by Alan on 11/07/2024.
// Based on: https://wiki.osdev.org/Interrupt_Descriptor_Table
//

#include "idt.h"
#include "display.h"
#include "kernel.h"

#define IDT_SIZE 256

// Helper functions for reading/writing from I/O
extern unsigned char inb(unsigned short port);
extern void outb(unsigned short port, unsigned char val);

struct InterruptDescriptor32 IDT[256];

void enable_interrupts(void) {
    __asm__ __volatile__("sti");
}

void disable_interrupts(void) {
    __asm__ __volatile__("cli");
}

void send_eoi(int irq) { // end of interrupt signal
    outb(0x20, 0x20);
}

void init_idt(void) {
    extern int load_idt(uint32_t*);
    extern int irq1();

    println("[IDT] Initializing IDT...");

    for (int i = 0; i < IDT_SIZE; i++) {
        IDT[i].offset_lowerbits = 0;
        IDT[i].selector = 0;
        IDT[i].zero = 0;
        IDT[i].type_attributes = 0;
        IDT[i].offset_higherbits = 0;
    }

    uint32_t idt_address = (uint32_t)IDT;
    uint32_t idt_ptr[2];
    uint16_t idt_limit = sizeof(struct InterruptDescriptor32) * IDT_SIZE - 1;
    idt_ptr[0] = idt_limit | ((idt_address & 0xFFFF) << 16);
    idt_ptr[1] = idt_address >> 16;

    print("[IDT] IDT address: 0x");
    print_uint32_hex(idt_ptr[0]);
    println("");

    load_idt(idt_ptr);

    println("[IDT] Initialized IDT");

    disable_interrupts();
    init_pic();
}

void init_pic(void) { // remap PIC
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);

    // disable all IRQs
    outb(0x21, 0xFF);
    outb(0xA1, 0xFF);
}

void enable_irq(uint8_t irq) {
    uint16_t port;
    uint8_t value;

    if (irq < 8) {    // master PIC (IRQ 0-7)
        port = 0x21;
    } else {          // slave PIC (IRQ 8-15)
        port = 0xA1;
        irq -= 8;
    }

    value = inb(port) & ~(1 << irq); // clear bit using bit mask(enable specific IRQ)
    outb(port, value);
}

void register_interrupt_handler(uint8_t interrupt_number, uint32_t handler_address) {
    IDT[interrupt_number].offset_lowerbits = handler_address & 0xffff;
    IDT[interrupt_number].selector = 0x08; // Code segment selector
    IDT[interrupt_number].zero = 0;
    IDT[interrupt_number].type_attributes = 0x8e;
    IDT[interrupt_number].offset_higherbits = (handler_address >> 16) & 0xffff;

    print("[IDT] Registered interrupt handler for IRQ");
    print_uint32(interrupt_number-32);
    println("");
}