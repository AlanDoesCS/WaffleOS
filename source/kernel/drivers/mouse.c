// A minimal implementation of a PS/2 mouse driver
// Using information from: https://wiki.osdev.org/PS/2_Mouse
// Adapted from: https://forum.osdev.org/viewtopic.php?t=10247
//

#include "mouse.h"
#include "../core/idt.h"
#include "../core/x86.h"
#include "../core/stdio.h"
#include "../core/kernel.h"
#include "display.h"  // For g_SCREEN_WIDTH and g_SCREEN_HEIGHT

// global mouse state
volatile MouseState mouse_state = { .x = 160, .y = 100, .buttons = 0 };

// Vars to accumulate the 3 byte mouse packet
static uint8_t mouse_cycle = 0;
static uint8_t mouse_bytes[3];

extern void irq12(void);

static inline void mouse_wait(uint8_t a_type)
{
    uint32_t _time_out = 100000;
    if(a_type == 0) {
        // Wait for data to be available
        while(_time_out--) {
            if((x86_inb(MOUSE_STATUS_PORT) & 1) == 1)
                return;
        }
    } else {
        // Wait for the input buffer to be empty
        while(_time_out--) {
            if((x86_inb(MOUSE_STATUS_PORT) & 2) == 0)
                return;
        }
    }
}

static inline void mouse_write(uint8_t a_write)
{
    mouse_wait(1);
    // Tell the controller we're sending a command to the mouse
    x86_outb(MOUSE_STATUS_PORT, 0xD4);
    mouse_wait(1);
    x86_outb(MOUSE_DATA_PORT, a_write);
}

uint8_t mouse_read()
{
    mouse_wait(0);
    return x86_inb(MOUSE_DATA_PORT);
}

void mouse_handler(void)
{
    printf("Mouse interrupt\r\n");
    switch(mouse_cycle)
    {
        case 0:
            mouse_bytes[0] = x86_inb(MOUSE_DATA_PORT);
            mouse_cycle++;
            break;
        case 1:
            mouse_bytes[1] = x86_inb(MOUSE_DATA_PORT);
            mouse_cycle++;
            break;
        case 2:
            mouse_bytes[2] = x86_inb(MOUSE_DATA_PORT);
            // Process the complete packet:
            // Byte 0: Button states and sign bits.
            //   Bit 0: Left button, Bit 1: Right button, Bit 2: Middle button.
            mouse_state.buttons = mouse_bytes[0] & 0x07;

            // Byte 1 and 2: Relative movement values.
            // Cast to int8_t to properly sign-extend the movement.
            int8_t x_move = (int8_t)mouse_bytes[1];
            int8_t y_move = (int8_t)mouse_bytes[2];

            // Update mouse coordinates (accumulate the relative movement)
            mouse_state.x += x_move;
            mouse_state.y += y_move;

            mouse_cycle = 0;
            break;
    }
    send_eoi(12);
}

void init_mouse(void)
{
    uint8_t _status;

    // Enable the auxiliary (mouse) device.
    mouse_wait(1);
    x86_outb(MOUSE_STATUS_PORT, 0xA8);

    // Enable the interrupts for the mouse.
    mouse_wait(1);
    x86_outb(MOUSE_STATUS_PORT, 0x20);
    mouse_wait(0);
    _status = (x86_inb(MOUSE_DATA_PORT) | 2);
    mouse_wait(1);
    x86_outb(MOUSE_STATUS_PORT, 0x60);
    mouse_wait(1);
    x86_outb(MOUSE_DATA_PORT, _status);

    // Set the mouse to its default settings.
    mouse_write(0xF6);
    mouse_read();  // Acknowledge

    // Enable the mouse device.
    mouse_write(0xF4);
    mouse_read();  // Acknowledge

    // Register the mouse interrupt handler.
    // IRQ 12 is remapped to interrupt number 44 (32 + 12)
    register_interrupt_handler(32 + 12, (uint32_t)mouse_handler);

    printf("[MOUSE] Mouse driver installed.\n");
}
