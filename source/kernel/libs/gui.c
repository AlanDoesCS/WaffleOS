#include "gui.h"
#include "../drivers/display.h"
#include "../drivers/cursor.h"
#include "../timers/timer.h"
#include <stdint.h>
#include <stddef.h>
#include <memory.h>

// Back buffer pointer.
static uint8_t *back_buffer = NULL;

// Allocate the back buffer. For mode 320x200 with 1 byte per pixel, that's 320*200 bytes.
void init_gui(void) {
    // Initialize the PIT if you need it.
    init_pit();
    // Allocate memory for the back buffer.
    back_buffer = (uint8_t *)malloc(g_SCREEN_WIDTH * g_SCREEN_HEIGHT * g_BYTES_PER_PIXEL);
    if (!back_buffer) {
        // Handle allocation error.
    }
}

static void clear_back_buffer(uint32_t color) {
    if (g_BYTES_PER_PIXEL == 1) {
        memset(back_buffer, (uint8_t)color, g_SCREEN_WIDTH * g_SCREEN_HEIGHT);
    }
}

static void flip_buffer(void) {
    // Copy the entire back buffer to the framebuffer.
    // Note: Make sure that 'framebuffer' is accessible; if it is static in display.c, consider adding a blit function there.
    memcpy((void*)display_get_framebuffer(), back_buffer, g_SCREEN_WIDTH * g_SCREEN_HEIGHT * g_BYTES_PER_PIXEL);
}

// Draw the GUI to the back buffer.
static void draw_gui_to_buffer(void) {
    // For these drawing functions to work on the back buffer, you can temporarily set the drawing target.
    // One approach is to modify put_pixel() to check a global flag or pointer (target_buffer) instead of drawing directly to framebuffer.
    // Here, we'll assume we've modified our drawing routines to draw into a pointer "current_buffer".

    // Set our drawing target to the back buffer.
    set_drawing_target(back_buffer);

    // Draw your GUI as before.
    draw_rect(0, 0, 319, 199, LIGHT_BLUE_16); // Draw background
    draw_rect(0, 0, 319, 20, BLUE_16);
    draw_string(3, 3, "Welcome to WaffleOS!", YELLOW_16);

    for (int i = 1; i < 10; i++) {
        float scale = 1.0f - (0.1f * (i - 1));
        draw_scaled_string(10, 20 + (i * 20), "Lorem ipsum dolor sit amet", RED_16, scale);
    }

    draw_rect(0, 180, 319, 199, BLUE_16);
    draw_scaled_string(5, 185, "Start", WHITE_16, 0.8f);

    // Draw the cursor last so it appears on top.
    draw_cursor();

    // Reset drawing target to the default framebuffer if needed.
    set_drawing_target(NULL);
}

// Main render function that uses double buffering.
void render_gui(void) {
    // Clear the back buffer (using BLACK_16 or any desired clear color).
    clear_back_buffer(BLACK_16);

    // Draw the entire GUI into the back buffer.
    draw_gui_to_buffer();

    // Blit the back buffer to the actual framebuffer.
    flip_buffer();
}
