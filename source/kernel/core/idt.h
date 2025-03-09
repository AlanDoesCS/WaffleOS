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

void i686_IDT_SetGate(int interrupt, void* base, uint16_t segmentDescriptor, uint8_t flags);
void i686_IDT_EnableGate(int interrupt);
void i686_IDT_DisableGate(int interrupt);

#define PIC1_CMD_PORT 0x20
#define PIC1_DATA_PORT 0x21
#define PIC2_CMD_PORT 0xA0
#define PIC2_DATA_PORT 0xA1
#define PIC_EOI 0x20

#define ICW1_INIT         0x11
#define ICW2_MASTER_OFFSET 0x20
#define ICW2_SLAVE_OFFSET  0x28
#define ICW3_MASTER        0x04
#define ICW3_SLAVE         0x02
#define ICW4_8086_MODE     0x01

// PIC1
#define IRQ0_BIT 0x01    // PIT
#define IRQ1_BIT 0x02    // Keyboard
#define IRQ2_BIT 0x04    // Cascade
#define IRQ3_BIT 0x08    // COM2
#define IRQ4_BIT 0x10    // COM1
#define IRQ5_BIT 0x20    // LPT2/Sound
#define IRQ6_BIT 0x40    // Floppy
#define IRQ7_BIT 0x80    // LPT1 (Parallel port)

// PIC2
#define IRQ8_BIT 0x01    // CMOS RTC
#define IRQ9_BIT 0x02    // Free/ACPI
#define IRQ10_BIT 0x04   // Free/Network
#define IRQ11_BIT 0x08   // Free/SCSI
#define IRQ12_BIT 0x10   // PS/2 Mouse
#define IRQ13_BIT 0x20   // FPU/Coprocessor
#define IRQ14_BIT 0x40   // Primary ATA
#define IRQ15_BIT 0x80   // Secondary ATA

void enable_interrupts(void);
void disable_interrupts(void);
void send_eoi(int irq);
void init_idt(void);
void init_pic(void);
void enable_irq(uint8_t irq);
void register_interrupt_handler(uint8_t interrupt_number, uint32_t handler_address);
