#include "cursor.h"
#include "display.h"
#include "mouse.h"     // for mouse_state

// Define a 8x8 arrow cursor
static const char *cursor_shape[8] = {
    "X       ",
    "XX      ",
    "X.X     ",
    "X..X    ",
    "X...X   ",
    "X.XXXX  ",
    "XX      ",
    "X       "
};


void draw_cursor(void) {
    int cursor_width = 8;
    int cursor_height = 8;

    int start_x = mouse_state.x;
    int start_y = mouse_state.y;

    for (int row = 0; row < cursor_height; row++) {
        for (int col = 0; col < cursor_width; col++) {
            if (cursor_shape[row][col] == '.') {
                put_pixel(start_x + col, start_y + row, WHITE_16);
            } else if (cursor_shape[row][col] == 'X') {
                put_pixel(start_x + col, start_y + row, BLACK_16);
            }
        }
    }
}
