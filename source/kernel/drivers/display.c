// Display driver
#include "display.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "../libs/string.h"
#include "../core/x86.h"
#include "../core/memory.h"
// #include "vbe_switch.h"
#include "font.h" // contains basic font definitions

uint16_t g_SCREEN_WIDTH = 320;  // Default value, overridden at runtime
uint16_t g_SCREEN_HEIGHT = 200; // Default value, overridden at runtime
uint16_t g_BYTES_PER_PIXEL = 1;

static uint32_t vbe_framebuffer;  // will hold the LFB address

// Global variables to hold the framebuffer pointer and pitch.
static volatile uint8_t *framebuffer = (volatile uint8_t *)0;
static uint32_t pitch;

void enable_graphics_mode(uint16_t mode) {
    framebuffer = (volatile uint8_t*)VGA_ADDRESS;

    if (mode > MODE_320x200x1 || mode < MODE_320x200x1) {
        mode = MODE_320x200x1; // revert to default if out of range
    }

    // Update screen variables
    GraphicsMode selectedMode = graphicsModes[mode];
    g_SCREEN_WIDTH = selectedMode.ScreenWidth;
    g_SCREEN_HEIGHT = selectedMode.ScreenHeight;
    g_BYTES_PER_PIXEL = selectedMode.BPP;  // BPP is in bytes already

    pitch = g_SCREEN_WIDTH;

    // write registers for specific graphics mode
    write_regs(registerInitTable[mode]);
}

// Clear the screen to the specified color.
void g_clrscr(uint32_t color) {
    for (int y = 0; y < g_SCREEN_HEIGHT; y++) {
        for (int x = 0; x < g_SCREEN_WIDTH; x++) {
            put_pixel(x, y, color);
        }
    }
}

void g_clear_screen() {
    for (int y = 0; y < g_SCREEN_HEIGHT; y++) {
        for (int x = 0; x < g_SCREEN_WIDTH; x++) {
            put_pixel(x, y, BLACK_16);
        }
    }
}

void draw_smile(int x, int y, uint32_t color) {
    // eye
    put_pixel(x, y, WHITE_16);
    // eye
    put_pixel(x+10, y, WHITE_16);
    // mouth
    put_pixel(x, y+8,WHITE_16);
    put_pixel(x+1,	y+9,WHITE_16);
    put_pixel(x+2,	y+10,WHITE_16);
    put_pixel(x+3,	y+10,WHITE_16);
    put_pixel(x+4,	y+10,WHITE_16);
    put_pixel(x+5,	y+10,WHITE_16);
    put_pixel(x+6,	y+10,WHITE_16);
    put_pixel(x+7,	y+10,WHITE_16);
    put_pixel(x+8,	y+10,WHITE_16);
    put_pixel(x+9,	y+9,WHITE_16);
    put_pixel(x+10,y+8,WHITE_16);
}

// Put a single pixel at the specified (x, y) position.
void put_pixel(int x, int y, uint32_t color) {
    // Bounds check
    if (x < 0 || x >= g_SCREEN_WIDTH || y < 0 || y >= g_SCREEN_HEIGHT)
        return;

    // If framebuffer is not zero, we can assume we have access to a linear framebuffer
    if (framebuffer) {
        uint32_t offset = y * pitch + x * g_BYTES_PER_PIXEL;
        switch (g_BYTES_PER_PIXEL) {
        case 1:
            ((volatile uint8_t*)framebuffer)[offset] = (uint8_t)color;
            break;
        case 2:
            *(volatile uint16_t*)(framebuffer + offset) = (uint16_t)color;
            break;
        case 4:
            *(volatile uint32_t*)(framebuffer + offset) = color;
            break;
        default:
            // Weird pixel format
            break;
        }
    } else {
        // Fall back to standard VGA mode:
        // VGA mode is fixed at 320x200 and uses an 8-bit indexed framebuffer at VGA_ADDRESS.
        uint32_t offset = y * g_SCREEN_WIDTH + x;
        ((volatile uint8_t*)VGA_ADDRESS)[offset] = (uint8_t)color;
    }
}


// Draw a filled rectangle at (x, y) with width and height, in the specified color.
void draw_rect(int x, int y, int width, int height, uint32_t color) {
    if (width <= 0 || height <= 0)
        return;
    // Simple clipping
    if (x < 0) { width += x; x = 0; }
    if (y < 0) { height += y; y = 0; }
    if (x + width > g_SCREEN_WIDTH)  width = g_SCREEN_WIDTH - x;
    if (y + height > g_SCREEN_HEIGHT) height = g_SCREEN_HEIGHT - y;
    if (width <= 0 || height <= 0)
        return;

    for (int j = 0; j < height; j++) {
        uint32_t offset = (y + j) * pitch + x * g_BYTES_PER_PIXEL;
        volatile uint8_t *row_ptr = framebuffer + offset;

        if (g_BYTES_PER_PIXEL == 1) {
            for (int i = 0; i < width; i++) {
                row_ptr[i] = (uint8_t)color;
            }
        } else if (g_BYTES_PER_PIXEL == 2) {
            volatile uint16_t *pixel_ptr = (volatile uint16_t*)row_ptr;
            for (int i = 0; i < width; i++) {
                pixel_ptr[i] = (uint16_t)color;
            }
        } else if (g_BYTES_PER_PIXEL == 4) {
            volatile uint32_t *pixel_ptr = (volatile uint32_t*)row_ptr;
            for (int i = 0; i < width; i++) {
                pixel_ptr[i] = color;
            }
        } else {
            // Fallback: use VGA_ADDRESS
            uint32_t offset = y * g_SCREEN_WIDTH + x;
            ((volatile uint8_t*)VGA_ADDRESS)[offset] = (uint8_t)color;
        }
    }
}

void draw_rectangle(int x, int y, int width, int height) {
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            put_pixel(x+i, y+j, GREEN_16);
        }
    }
}


// draw_line: Draw a line from (x1, y1) to (x2, y2) in the given color using Bresenham's algorithm.
void draw_line(int x1, int y1, int x2, int y2, uint32_t color) {
    int dx = (x2 > x1 ? x2 - x1 : x1 - x2);
    int dy = (y2 > y1 ? y2 - y1 : y1 - y2);
    int sx = (x1 < x2 ? 1 : -1);
    int sy = (y1 < y2 ? 1 : -1);
    int err = dx - dy;

    while (1) {
        put_pixel(x1, y1, color);
        if (x1 == x2 && y1 == y2)
            break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x1 += sx; }
        if (e2 < dx) { err += dx; y1 += sy; }
    }
}

// Draw a single character at (x, y) using the 8x16 font.
// NOTE: This function assumes that the character is in the ASCII range 0-127, and does NOT support scaling
void draw_char(int x, int y, char ch, uint32_t color) {
    uint8_t c = (uint8_t)ch;  // Convert to unsigned value.
    for (int row = 0; row < 16; row++) {
        uint8_t bits = font8x16[c][row];
        for (int col = 0; col < 8; col++) {
            if (bits & (0x80 >> col)) {
                put_pixel(x + col, y + row, color);
            }
        }
    }
}

// Draw a null-terminated string starting at (x, y).
// NOTE: This function assumes that the character is in the ASCII range 0-127, and does NOT support scaling
void draw_string(int x, int y, const char *str, uint32_t color) {
    while (*str) {
        if (*str == '\n') {
            y += 16;  // Move down by the font height
            x = 0;    // Reset to the left margin
        } else {
            draw_char(x, y, *str, color);
            x += 8;   // Advance by font width
        }
        str++;
    }
}

// Scaling--------------------------------------------------------------------------------------------------------------
// These functions are slightly modified in order to support scaling of the font.

// Draw a single scaled character at (x, y).
// 'ch' is the character, 'color' is the pixel color, and 'scale' is the scaling factor.
// NOTE: This function assumes that the character is in the ASCII range 0-127.
void draw_scaled_char(int x, int y, char ch, uint32_t color, int scale) {
    uint8_t c = (uint8_t)ch;  // Convert to unsigned value.
    for (int row = 0; row < 16; row++) {
        uint8_t bits = font8x16[c][row];
        for (int col = 0; col < 8; col++) {
            if (bits & (0x80 >> col)) {
                // Draw a block of scale x scale pixels for each "on" bit.
                for (int dy = 0; dy < scale; dy++) {
                    for (int dx = 0; dx < scale; dx++) {
                        put_pixel(x + col * scale + dx, y + row * scale + dy, color);
                    }
                }
            }
        }
    }
}

// Draw a null-terminated string starting at (x, y) with scaling.
// NOTE: This function assumes that the character is in the ASCII range 0-127.
void draw_scaled_string(int x, int y, const char *str, uint32_t color, int scale) {
    while (*str) {
        if (*str == '\n') {
            y += 16 * scale;  // Move down by the scaled font height.
            x = 0;            // Reset to the left margin.
        } else {
            draw_scaled_char(x, y, *str, color, scale);
            x += 8 * scale;   // Advance by the scaled font width.
        }
        str++;
    }
}
