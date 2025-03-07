// A minimal implementation of a PS/2 mouse driver
// Source: https://wiki.osdev.org/PS/2_Mouse
#pragma once

#include <stdint.h>
#include <stdbool.h>

// PS/2 ports for mouse.
#define MOUSE_DATA_PORT    0x60
#define MOUSE_STATUS_PORT  0x64

// Mouse state struct
typedef struct {
    int x;            // Current x pos
    int y;            // Current y pos
    uint8_t buttons; // Button state: bit0 = left, bit1 = right, bit2 = middle
} MouseState;

extern volatile MouseState mouse_state;

void init_mouse(void);
void mouse_handler(void);
