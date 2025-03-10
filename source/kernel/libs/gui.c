#include "gui.h"
#include "../drivers/display.h"
#include "icons.h"
#include "cursor.h"
#include "../timers/timer.h"
#include "string.h"
#include <stdint.h>
#include <stddef.h>
#include <memory.h>

// Back buffer pointer.
static uint8_t *back_buffer = NULL;

// GUI rendering Functions ---------------------------------------------------------------------------------------------

void init_gui(void) {
    init_pit();
    .
    back_buffer = (uint8_t *)malloc(g_SCREEN_WIDTH * g_SCREEN_HEIGHT * g_BYTES_PER_PIXEL);
    if (!back_buffer) {
        // Allocation error
    }
}

static void clear_back_buffer(uint32_t color) {
    if (g_BYTES_PER_PIXEL == 1) {
        memset(back_buffer, (uint8_t)color, g_SCREEN_WIDTH * g_SCREEN_HEIGHT);
    }
}

static void flip_buffer(void) {
    memcpy((void*)display_get_framebuffer(), back_buffer, g_SCREEN_WIDTH * g_SCREEN_HEIGHT * g_BYTES_PER_PIXEL);
}

// Draw the GUI to the back buffer.
static void draw_gui_to_buffer(void) {

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

    draw_all_windows();

    // DEBUG: draw icons -----------------------------------------------------------------------------------------------
    draw_rect(9, 24, 70, 10, WHITE_16); // Draw background

    // test draw icons
    draw_icon(10, 25, util_icons[SMILEY]);
    draw_icon(20, 25, util_icons[FROWNY]);
    draw_icon(30, 25, util_icons[HEART]);
    draw_icon(40, 25, util_icons[FLOPPY]);
    draw_icon(50, 25, util_icons[INFO]);
    draw_icon(60, 25, util_icons[ERROR]);
    draw_icon(70, 25, util_icons[GEAR]);
    // -----------------------------------------------------------------------------------------------------------------

    // Draw the cursor last so it appears on top.
    draw_cursor();

    // Reset drawing target to the default framebuffer
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

// Window Managing -----------------------------------------------------------------------------------------------------
static Window* windows[MAX_WINDOWS];
static int window_count = 0;

Window* create_window(int x, int y, int width, int height, uint32_t bg_color, const char *title) {
    if (window_count >= MAX_WINDOWS) {
        return NULL;  // No more windows available.
    }
    Window *win = (Window*)malloc(sizeof(Window));
    if (!win)
        return NULL;

    win->x = x;
    win->y = y;
    win->width = width;
    win->height = height;
    win->bg_color = bg_color;
    strncpy(win->title, title, sizeof(win->title) - 1);
    win->title[sizeof(win->title) - 1] = '\0';
    win->is_active = true;

    windows[window_count++] = win;
    return win;
}

// Destroy a window and free its memory.
void destroy_window(Window *win) {
    if (!win)
        return;
    int index = -1;
    for (int i = 0; i < window_count; i++) {
        if (windows[i] == win) {
            index = i;
            break;
        }
    }
    if (index == -1)
        return;

    free(win);
    // Shift remaining windows down.
    for (int i = index; i < window_count - 1; i++) {
        windows[i] = windows[i + 1];
    }
    window_count--;
}

// Move an existing window to a new (x, y) position.
void move_window(Window *win, int new_x, int new_y) {
    if (win) {
        win->x = new_x;
        win->y = new_y;
    }
}

// Resize an existing window.
void resize_window(Window *win, int new_width, int new_height) {
    if (win) {
        win->width = new_width;
        win->height = new_height;
    }
}

// Bring the given window to the front (draw it last so it appears on top).
void bring_to_front(Window *win) {
    if (!win)
        return;
    int index = -1;
    for (int i = 0; i < window_count; i++) {
        if (windows[i] == win) {
            index = i;
            break;
        }
    }
    if (index == -1 || index == window_count - 1)
        return; // Already on top.

    // Shift the window forward.
    for (int i = index; i < window_count - 1; i++) {
        windows[i] = windows[i + 1];
    }
    windows[window_count - 1] = win;
}

// Draw a single window.
void draw_window(Window *win) {
    if (!win)
        return;

    // Draw window background.
    draw_rect(win->x, win->y, win->width, win->height, win->bg_color);

    // Draw window border (using a 1-pixel white border).
    for (int i = 0; i < win->width; i++) {
        put_pixel(win->x + i, win->y, WHITE_16);                    // Top border
        put_pixel(win->x + i, win->y + win->height - 1, WHITE_16);     // Bottom border
    }
    for (int j = 0; j < win->height; j++) {
        put_pixel(win->x, win->y + j, WHITE_16);                     // Left border
        put_pixel(win->x + win->width - 1, win->y + j, WHITE_16);      // Right border
    }

    // Draw the title bar.
    int title_bar_height = 16;  // Fixed height for the title bar.
    draw_rect(win->x, win->y, win->width, title_bar_height, BLUE_16);
    draw_string(win->x + 4, win->y + 4, win->title, WHITE_16);
}

// Draw all active windows.
void draw_all_windows(void) {
    // Draw windows in order (earlier windows are drawn first, later ones on top).
    for (int i = 0; i < window_count; i++) {
        if (windows[i]->is_active) {
            draw_window(windows[i]);
        }
    }
}
