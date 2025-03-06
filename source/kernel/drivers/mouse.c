// A minimal implementation of a PS/2 mouse driver
// Source: https://wiki.osdev.org/PS/2_Mouse
#include "mouse.h"
#include "../core/idt.h"
#include "../core/x86.h"
#include "../core/stdio.h"
#include "../core/kernel.h"
#include "display.h"  // For g_SCREEN_WIDTH and g_SCREEN_HEIGHT

// PS/2 ports for mouse.
#define MOUSE_DATA_PORT    0x60
#define MOUSE_STATUS_PORT  0x64

extern void irq12(void);

// global mouse state used in display.c
volatile MouseState mouse_state = { .x = 160, .y = 100, .buttons = 0 };

// Vars to accumulate the 3 byte mouse packet
static int mouse_cycle = 0;
static uint8_t mouse_packet[3] = {0};

// PS/2 mouse interrupt handler
void mouse_handler(void) {
    uint8_t data = x86_inb(MOUSE_DATA_PORT);
    mouse_packet[mouse_cycle++] = data;

    // Process the packet when all 3 bytes are received
    if (mouse_cycle == 3) {
        // Byte 0: button states
        // Byte 1: x
        // Byte 2: y
        int x_move = (int8_t)mouse_packet[1];
        int y_move = (int8_t)mouse_packet[2];

        mouse_state.x += x_move;
        mouse_state.y -= y_move;

        // Clamp coords
        if (mouse_state.x < 0) mouse_state.x = 0;
        if (mouse_state.y < 0) mouse_state.y = 0;
        if (mouse_state.x >= g_SCREEN_WIDTH)  mouse_state.x = g_SCREEN_WIDTH - 1;
        if (mouse_state.y >= g_SCREEN_HEIGHT) mouse_state.y = g_SCREEN_HEIGHT - 1;

        // Update button states (only lower three bits are used).
        mouse_state.buttons = mouse_packet[0] & 0x07;

        // Reset cycle for next packet.
        mouse_cycle = 0;
    }

    // Signal end of interrupt for IRQ 12.
    send_eoi(12);
}

// Initialize the PS/2 mouse.
void init_mouse(void) {
    printf("[I/O] Initializing mouse...\r\n");

    // Enable the auxiliary device (mouse) by sending 0xA8 to port 0x64.
    while (x86_inb(MOUSE_STATUS_PORT) & 0x02);
    x86_outb(MOUSE_STATUS_PORT, 0xA8);

    // Enable mouse IRQ in the command byte.
    while (x86_inb(MOUSE_STATUS_PORT) & 0x02);
    x86_outb(MOUSE_STATUS_PORT, 0x20);
    uint8_t status = x86_inb(MOUSE_DATA_PORT);
    status |= 2;  // Set bit 1 to enable IRQ12.
    while (x86_inb(MOUSE_STATUS_PORT) & 0x02);
    x86_outb(MOUSE_STATUS_PORT, 0x60);
    while (x86_inb(MOUSE_STATUS_PORT) & 0x02);
    x86_outb(MOUSE_DATA_PORT, status);

    // Tell the controller we want to send a command to the mouse.
    while (x86_inb(MOUSE_STATUS_PORT) & 0x02);
    x86_outb(MOUSE_STATUS_PORT, 0xD4);
    // Set mouse defaults.
    while (x86_inb(MOUSE_STATUS_PORT) & 0x02);
    x86_outb(MOUSE_DATA_PORT, 0xF6);
    x86_inb(MOUSE_DATA_PORT);  // Discard ACK.

    // Enable data reporting.
    while (x86_inb(MOUSE_STATUS_PORT) & 0x02);
    x86_outb(MOUSE_STATUS_PORT, 0xD4);
    while (x86_inb(MOUSE_STATUS_PORT) & 0x02);
    x86_outb(MOUSE_DATA_PORT, 0xF4);
    x86_inb(MOUSE_DATA_PORT);  // Discard ACK.

    // Register the mouse interrupt handler (IRQ 12 is vector 44).
    register_interrupt_handler(44, (uint32_t)irq12);
    // Enable IRQ 12.
    enable_irq(12);

    printf("[I/O] Mouse initialized\r\n");
}
