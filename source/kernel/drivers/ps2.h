#pragma once

#include <stdint.h>

#define PS2_STATUS_PORT   0x64
#define PS2_DATA_PORT     0x60

#define PS2_CMD_READ_CONFIG    0x20
#define PS2_CMD_WRITE_CONFIG   0x60
#define PS2_CMD_ENABLE_MOUSE   0xA8

#define PS2_WAIT_FOR_DATA  0  // Wait for data available (bit0 set)
#define PS2_WAIT_FOR_EMPTY 1  // Wait for input buffer to be empty (bit1 clear)

void ps2_wait(uint8_t type);
void ps2_write_command(uint16_t port, uint8_t data);
uint8_t ps2_read_data(uint16_t port);
void ps2_flush_buffer(void);

// Device-specific I/O via the PS/2 Controller -------------------------------------------------------------------------

void ps2_write_to_device(uint8_t data);
uint8_t ps2_read_from_device(void);
