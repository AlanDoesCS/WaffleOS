//
// Created by Alan on 14/07/2024.
// Based on: https://wiki.osdev.org/Programmable_Interval_Timer
//

#include "types.h"
#include "timer.h"
#include "display.h"
#include "idt.h"

#define PIT_FREQUENCY 1193182
#define TARGET_FREQUENCY 100     // Hz
#define PIT_COMMAND 0x36
#define PIT_CHANNEL_0 0x40
#define PIT_COMMAND_PORT 0x43

// Helper functions for reading/writing from I/O
extern unsigned char inb(unsigned short port);
extern void outb(unsigned short port, unsigned char val);

extern void irq0(void);

// 64 bit tick count
static volatile uint32_t milliseconds_low = 0;
static volatile uint32_t milliseconds_high = 0;

void init_pit() {
    uint32_t divisor = PIT_FREQUENCY / TARGET_FREQUENCY;
    print("[PIT] Divisor: ");
    print_uint32(divisor);
    println("");

    outb(PIT_COMMAND_PORT, PIT_COMMAND);
    outb(PIT_CHANNEL_0, (uint8_t)(divisor & 0xFF));
    outb(PIT_CHANNEL_0, (uint8_t)((divisor >> 8) & 0xFF));

    register_interrupt_handler(32, (uint32_t)irq0); // IRQ0 is interrupt 32

    enable_irq(0); // Enable IRQ0

    println("[PIT] Programmable Interval Timer initialized");
}

void timer_handler(void) {
    milliseconds_low++;
    if (milliseconds_low == 0) {  // Overflow occurred
        milliseconds_high++;
    }

    print_char('.');

    if (milliseconds_low % TARGET_FREQUENCY == 0) {
        print("[PIT] 1 second has passed, system millis: ");
        print_uint32(milliseconds_low);
        println("");
    }

    send_eoi(0);
}

void sleep_millis(uint32_t duration) {
    uint32_t start_low = milliseconds_low;
    uint32_t start_high = milliseconds_high;

    uint32_t end_low = start_low + duration;
    uint32_t end_high = start_high + (end_low < start_low ? 1 : 0);

    while (milliseconds_high < end_high || ((milliseconds_high == end_high) && (milliseconds_low < end_low))) {
        __asm__ volatile("hlt");
    }
    send_eoi(0);
}

void sleep(uint32_t duration) {
    sleep_millis(duration * 1000);
}

void get_milliseconds(uint32_t* low, uint32_t* high) {
    disable_interrupts();

    *low = milliseconds_low;
    *high = milliseconds_high;

    enable_interrupts();
}

uint32_t get_seconds(void) {
    uint32_t low, high;
    get_milliseconds(&low, &high);
    return low / 1000 + high * 4294967; // 4294967 = (2^32 / 1000)
}