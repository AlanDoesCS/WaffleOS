// Display driver
#include "display.h"
#include <stddef.h>
#include <stdint.h>

#include "../libs/string.h"
#include "../core/x86.h"
#include "../core/memory.h"
// #include "vbe_switch.h"
#include "font8x16.h" // contains a basic 8x16 font in a 2D array

#define SCREEN_WIDTH   640
#define SCREEN_HEIGHT  480
#define BYTES_PER_PIXEL 4    // 4 for 32-bit color (each pixel is 4 bytes)

static uint32_t vbe_framebuffer;  // will hold the LFB address

// Global variables to hold the framebuffer pointer and pitch.
static volatile uint8_t *framebuffer = (volatile uint8_t *)0;
static uint32_t pitch = SCREEN_WIDTH * BYTES_PER_PIXEL;

void enable_graphics() {
    //vbe_framebuffer = set_vbe_mode_640x480x32();
    //display_init(vbe_framebuffer, SCREEN_WIDTH * BYTES_PER_PIXEL);
}

void display_init(uint32_t framebuffer_addr, uint32_t pitch_val) {
    framebuffer = (volatile uint8_t *)framebuffer_addr;
    pitch = pitch_val;
}

// Clear the screen to the specified color.
void g_clrscr(uint32_t color) {
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            putpixel(x, y, color);
        }
    }
}

// Plot a pixel at (x, y) with the specified 32-bit color.
// For 32-bit mode, color is in 0x00RRGGBB format.
void putpixel(int x, int y, uint32_t color) {
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT)
        return;
    uint32_t offset = y * pitch + x * BYTES_PER_PIXEL;
    *(uint32_t*)(framebuffer + offset) = color;
}

// Draw a filled rectangle at (x, y) with width and height, in the specified color.
void draw_rect(int x, int y, int width, int height, uint32_t color) {
    if (width <= 0 || height <= 0)
        return;
    // Simple clipping
    if (x < 0) { width += x; x = 0; }
    if (y < 0) { height += y; y = 0; }
    if (x + width > SCREEN_WIDTH)  width = SCREEN_WIDTH - x;
    if (y + height > SCREEN_HEIGHT) height = SCREEN_HEIGHT - y;
    if (width <= 0 || height <= 0)
        return;

    volatile uint8_t *row_ptr = framebuffer + y * pitch + x * BYTES_PER_PIXEL;
    for (int j = 0; j < height; j++) {
        uint32_t *pixel_ptr = (uint32_t*)row_ptr;
        for (int i = 0; i < width; i++) {
            pixel_ptr[i] = color;
        }

        row_ptr += pitch;
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
        putpixel(x1, y1, color);
        if (x1 == x2 && y1 == y2)
            break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x1 += sx; }
        if (e2 < dx) { err += dx; y1 += sy; }
    }
}

// Draw a single character at (x, y) using the built-in 8x16 bitmap font.
// (x, y) is the top-left corner where the character is drawn.
void draw_char(int x, int y, char ch, uint32_t color) {
    uint8_t c = (uint8_t)ch;  // Convert to unsigned value.
    for (int row = 0; row < 16; row++) {
        uint8_t bits = font8x16[c][row];
        for (int col = 0; col < 8; col++) {
            if (bits & (0x80 >> col)) {
                putpixel(x + col, y + row, color);
            }
        }
    }
}

// Draw a null-terminated string starting at (x, y).
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
