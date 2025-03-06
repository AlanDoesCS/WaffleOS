//
// Interrupt Descriptor Table
// Based on: https://wiki.osdev.org/Interrupt_Descriptor_Table
//
// Some code is from nanobye's IDT implementation:
// https://github.com/nanobyte-dev/nanobyte_os/blob/videos/part8/src/kernel/arch/i686/idt.c
//

#include "idt.h"
#include "stdio.h"
#include "kernel.h"
#include "x86.h"

#include <utils/binary.h>

#define IDT_SIZE 256

// Start of code by nanobyte
typedef struct
{
    uint16_t BaseLow;            // offset bits 0..15
    uint16_t SegmentSelector;    // a code segment selector in GDT
    uint8_t Reserved;            // reserved/unused, set to 0
    uint8_t Flags;               // gate type, dpl, and p fields
    uint16_t BaseHigh;           // offset bits 16..31
} __attribute__((packed)) IDTEntry;

typedef struct
{
    uint16_t Limit;
    IDTEntry* Ptr;
} __attribute__((packed)) IDTDescriptor;

IDTEntry g_IDT[256];
IDTDescriptor g_IDTDescriptor = { sizeof(g_IDT) - 1, g_IDT };

void __attribute__((cdecl)) i686_IDT_Load(IDTDescriptor* idtDescriptor);

void i686_IDT_SetGate(int interrupt, void* base, uint16_t segmentDescriptor, uint8_t flags)
{
    g_IDT[interrupt].BaseLow = ((uint32_t)base) & 0xFFFF;
    g_IDT[interrupt].SegmentSelector = segmentDescriptor;
    g_IDT[interrupt].Reserved = 0;
    g_IDT[interrupt].Flags = flags;
    g_IDT[interrupt].BaseHigh = ((uint32_t)base >> 16) & 0xFFFF;
}

void i686_IDT_EnableGate(int interrupt)
{
    FLAG_SET(g_IDT[interrupt].Flags, IDT_FLAG_PRESENT);
}

void i686_IDT_DisableGate(int interrupt)
{
    FLAG_UNSET(g_IDT[interrupt].Flags, IDT_FLAG_PRESENT);
}
// End of code by nanobyte

void enable_interrupts(void) {
    __asm__ __volatile__("sti");
}

void disable_interrupts(void) {
    __asm__ __volatile__("cli");
}

void send_eoi(int irq) { // end of interrupt signal
    x86_outb(0x20, 0x20);
}

void init_idt(void) {

    printf("[IDT] Initializing IDT...\r\n");

    for (int i = 0; i < IDT_SIZE; i++) {
        g_IDT[i].BaseLow = 0;
        g_IDT[i].SegmentSelector = 0;
        g_IDT[i].Reserved = 0;
        g_IDT[i].Flags = 0;
        g_IDT[i].BaseHigh = 0;
    }

    printf("[IDT] IDT address: 0x%x\r\n", (uint32_t)g_IDTDescriptor.Ptr);

    i686_IDT_Load(&g_IDTDescriptor);

    printf("[IDT] Initialized IDT\r\n");

    disable_interrupts();
    init_pic();
}

void init_pic(void) { // remap PIC
    x86_outb(0x20, 0x11);
    x86_outb(0xA0, 0x11);
    x86_outb(0x21, 0x20);
    x86_outb(0xA1, 0x28);
    x86_outb(0x21, 0x04);
    x86_outb(0xA1, 0x02);
    x86_outb(0x21, 0x01);
    x86_outb(0xA1, 0x01);

    // disable all IRQs
    x86_outb(0x21, 0xFF);
    x86_outb(0xA1, 0xFF);
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

    value = x86_inb(port);
    // Clear the IRQ bit using bit mask
    FLAG_UNSET(value, (1 << irq));
    x86_outb(port, value);
}

void register_interrupt_handler(uint8_t interrupt_number, uint32_t handler_address) {
    g_IDT[interrupt_number].BaseLow = handler_address & 0xFFFF;
    g_IDT[interrupt_number].SegmentSelector = 0x08; // Code segment selector
    g_IDT[interrupt_number].Reserved = 0;
    g_IDT[interrupt_number].Flags = 0x8E;
    g_IDT[interrupt_number].BaseHigh = (handler_address >> 16) & 0xFFFF;

    printf("[IDT] Registered interrupt handler for IRQ %d\r\n", interrupt_number-32);
}