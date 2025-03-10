#pragma once
#include <stdint.h>
#include <stdbool.h>

#define MAX_WINDOWS 10

typedef struct Window {
    int x;              // Top-left x coordinate
    int y;              // Top-left y coordinate
    int width;          // Window width in pixels
    int height;         // Window height in pixels
    uint32_t bg_color;  // Background color
    char title[128];    // Window title (max 127 characters + null terminator)
    bool is_active;     // Indicates if the window is active (visible)
} Window;

void init_gui(void);
void render_gui(void);

// Window management
void draw_all_windows(void);
