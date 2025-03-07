#include "ps2.h"
#include <x86.h>

void ps2_wait(uint8_t type) {
    uint32_t timeout = 100000;
    if (type == PS2_WAIT_FOR_DATA) {
        while (timeout--) {
            if (x86_inb(PS2_STATUS_PORT) & 1)
                return;
        }
    } else { // PS2_WAIT_FOR_EMPTY
        while (timeout--) {
            if ((x86_inb(PS2_STATUS_PORT) & 2) == 0)
                return;
        }
    }
}

void ps2_write_command(uint16_t port, uint8_t data) {
    ps2_wait(PS2_WAIT_FOR_EMPTY);
    x86_outb(port, data);
}

uint8_t ps2_read_data(uint16_t port) {
    ps2_wait(PS2_WAIT_FOR_DATA);
    return x86_inb(port);
}

void ps2_flush_buffer(void) {
    // Flush any remaining data in the controller's output buffer.
    while (x86_inb(PS2_STATUS_PORT) & 1) {
        (void)x86_inb(PS2_DATA_PORT);
    }
}

void ps2_write_to_device(uint8_t data) {
    // Tell the PS/2 controller that the following byte is for a device (e.g. mouse).
    ps2_wait(PS2_WAIT_FOR_EMPTY);
    x86_outb(PS2_STATUS_PORT, 0xD4);
    ps2_wait(PS2_WAIT_FOR_EMPTY);
    x86_outb(PS2_DATA_PORT, data);
}

uint8_t ps2_read_from_device(void) {
    return ps2_read_data(PS2_DATA_PORT);
}
