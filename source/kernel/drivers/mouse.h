// A minimal implementation of a PS/2 mouse driver
// Source: https://wiki.osdev.org/PS/2_Mouse
#pragma once

#include <stdint.h>
#include <stdbool.h>

// PS/2 ports for mouse.
#define MOUSE_DATA_PORT          0x60
#define MOUSE_STATUS_PORT        0x64

#define PS2_CMD_ENABLE_MOUSE    0xA8
#define PS2_CMD_READ_CONFIG     0x20
#define PS2_CMD_WRITE_CONFIG    0x60
#define MOUSE_CMD_SET_DEFAULTS  0xF6
#define MOUSE_CMD_ENABLE        0xF4
#define MOUSE_CMD_RESET         0xFF
#define MOUSE_ACK               0xFA
#define MOUSE_SELF_TEST_PASS    0xAA

// Mouse state struct
typedef struct {
    int x;            // Current x pos
    int y;            // Current y pos
    uint8_t buttons; // Button state: bit0 = left, bit1 = right, bit2 = middle
} MouseState;

extern volatile MouseState mouse_state;

/*
* Initialize the mouse driver.
* Return codes:
* `0`:	Success
* `-1`:	Failure
 */
int init_mouse(void);
void mouse_handler(void);
