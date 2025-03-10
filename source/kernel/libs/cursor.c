#include "cursor.h"
#include "icons.h"
#include "../drivers/display.h"
#include "../drivers/mouse.h"     // for mouse_state

// Global variable to track the current cursor type
static CursorType current_cursor = CURSOR_DEFAULT;

// getter and setter for the current cursor type

void set_cursor(CursorType new_cursor) {
    current_cursor = new_cursor;
}

// Draw the current cursor at the current mouse position, to the screen
void draw_cursor(void) {
    int start_x = mouse_state.x;
    int start_y = mouse_state.y;

    // Get the current cursor icon from the lookup table defined in icons.h
    const char (*icon)[8] = cursor_shapes[current_cursor];

    // Use draw_icon to handle drawing the cursor
    draw_icon(start_x, start_y, icon);
}
