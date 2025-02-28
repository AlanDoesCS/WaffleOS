//
// Created by Alan on 11/07/2024.
// Based on: https://wiki.osdev.org/Printing_To_Screen
//

#include "../types.h"
#include "../libs/str.h"

#define VIDEO_MEMORY 0xb8000

// Color Table
#define BLACK 0x0
#define BLUE 0x1
#define GREEN 0x2
#define CYAN 0x3
#define RED 0x4
#define MAGENTA 0x5
#define BROWN 0x6
#define LIGHT_GRAY 0x7
#define DARK_GRAY 0x8
#define LIGHT_BLUE 0x9
#define LIGHT_GREEN 0xa
#define LIGHT_CYAN 0xb
#define LIGHT_RED 0xc
#define LIGHT_MAGENTA 0xd
#define YELLOW 0xe
#define WHITE 0xf

#define WHITE_ON_BLACK 0x0f
#define WHITE_ON_BLUE 0x1f

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

#define TAB_SIZE 4

// current color combination being used
uint8_t COLOR = WHITE_ON_BLUE;

// Helper functions for reading/writing from I/O
extern unsigned char inb(unsigned short port);
extern void outb(unsigned short port, unsigned char val);

uint16_t current_row = 0;
uint16_t current_col = 0;

uint16_t get_cursor_col() {
    return current_col;
}

uint16_t get_cursor_row() {
    return current_row;
}

void move_cursor(int row, int col) {
    current_row = row;
    current_col = col;

    // Calculate the position in the VGA text mode buffer
    int position = (row) * SCREEN_WIDTH + col;

    outb(0x3D4, 0x0F);          // Low cursor control register index
    outb(0x3D5, (unsigned char)(position & 0xFF)); // Low byte of position
    outb(0x3D4, 0x0E);          // High cursor control register index
    outb(0x3D5, (unsigned char)((position >> 8) & 0xFF)); // High byte of position
}

void clear() {
    volatile char* video_memory = (volatile char*)VIDEO_MEMORY;
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        *video_memory++ = ' ';
        *video_memory++ = COLOR;
    }

    // go back to start
    move_cursor(0, 0);
}

void scroll_screen() {
    volatile char* video_memory = (volatile char*)VIDEO_MEMORY;

    // Move each line up by one
    for (int i = 0; i < (SCREEN_HEIGHT - 1) * SCREEN_WIDTH; i++) {
        video_memory[i * 2] = video_memory[(i + SCREEN_WIDTH) * 2];
        video_memory[i * 2 + 1] = video_memory[(i + SCREEN_WIDTH) * 2 + 1];
    }

    // Clear the last line
    for (int i = (SCREEN_HEIGHT - 1) * SCREEN_WIDTH; i < SCREEN_HEIGHT * SCREEN_WIDTH; i++) {
        video_memory[i * 2] = ' ';
        video_memory[i * 2 + 1] = COLOR;
    }
}

void print_char_xy(unsigned char c, int col, int row) {
    col = col % SCREEN_WIDTH;
    row = row % SCREEN_HEIGHT;

    volatile unsigned char* video_memory = (volatile unsigned char*)VIDEO_MEMORY;
    int offset = 2 * (row * SCREEN_WIDTH + col);
    video_memory[offset] = c;
    video_memory[offset + 1] = COLOR;
}

void del_last_char() {
    if (current_col == 0) {
        return;
    }

    current_col--;
    print_char_xy('\0', current_col, current_row);
    move_cursor(current_row, current_col);
}

void print_char(unsigned char c) {
    if (c == '\n') {
        current_col = 0;
        current_row++;
    } else if (c == '\t') {
        current_col += TAB_SIZE-(current_col % TAB_SIZE);
    } else if (c == '\b') {
        del_last_char();
        return;
    } else {

        if (current_col >= SCREEN_WIDTH) {
            current_col=0;
            current_row++;
        }

        if (current_row >= SCREEN_HEIGHT) {
            scroll_screen();
            current_row = SCREEN_HEIGHT - 1;
        }

        print_char_xy(c, current_col, current_row);
        current_col++;
    }

    move_cursor(current_row, current_col);
}

void print_uint8(uint8_t value) {
    if (value == 0) {
        print_char('0');
        return;
    }

    char buffer[3];
    int i = 2;

    while (value > 0 && i >= 0) {
        buffer[i] = (value % 10) + '0';  // Convert digit to ASCII
        value /= 10;
        i--;
    }

    i++;
    while (i < 3) {
        print_char(buffer[i]);
        i++;
    }
}

void print_uint16(uint16_t value) {
    if (value == 0) {
        print_char('0');
        return;
    }

    char buffer[5];
    int i = 4;

    while (value > 0 && i >= 0) {
        buffer[i] = (value % 10) + '0';  // Convert digit to ASCII
        value /= 10;
        i--;
    }

    i++;
    while (i < 5) {
        print_char(buffer[i]);
        i++;
    }
}

void print_uint32(uint32_t value) {
    if (value == 0) {
        print_char('0');
        return;
    }

    char buffer[10];
    int i = 9;

    while (value > 0 && i >= 0) {
        buffer[i] = (value % 10) + '0';  // Convert digit to ASCII
        value /= 10;
        i--;
    }

    i++;
    while (i < 10) {
        print_char(buffer[i]);
        i++;
    }
}

void print(const char* str) {
    for(int i=0; str[i] != 0; i++) {
        print_char(str[i]);
    }
}

void print_uint32_hex(uint32_t value) {
    char buffer[9];
    int_to_hex_str(value, buffer);
    print(buffer);
}

void print_hex(uint32_t value, int num_digits) {
    char buffer[9];
    int_to_hex_str(value, buffer);

    // Pad with zeros
    int start = 8 - num_digits;
    if (start < 0) start = 0;

    for (int i = start; buffer[i] != '\0'; i++) {
        print_char(buffer[i]);
    }
}

void println(const char* str) {
    print(str);
    print_char('\n');
}