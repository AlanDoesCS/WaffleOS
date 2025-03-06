// A minimal implementation of a PS/2 mouse driver
// Source: https://wiki.osdev.org/PS/2_Mouse
#pragma once

#include <stdint.h>
#include <stdbool.h>

// Mouse state struct
typedef struct {
    int x;            // Current x pos
    int y;            // Current y pos
    uint8_t buttons; // Button state: bit0 = left, bit1 = right, bit2 = middle
} MouseState;

extern volatile MouseState mouse_state;

// Initialize the mouse.
void init_mouse(void);

// Mouse interrupt handler.
void mouse_handler(void);
