#include "icons.h"
#include <stdio.h>
#include "../drivers/display.h"
#include "../drivers/vga.h" // for color macros

const ICON cursor_shapes[CURSOR_TOTAL] = {
    { // CURSOR_DEFAULT
        "X       ",
        "XX      ",
        "X.X     ",
        "X..X    ",
        "X...X   ",
        "X.XXXX  ",
        "XX      ",
        "X       "
    },
    { // CURSOR_HAND
        "  X     ",
        " X.X    ",
        " X.XX   ",
        "XX.X.XX ",
        "XX.X.X.X",
        "X......X",
        " X....X ",
        "  XXXX  "
    },
    { // CURSOR_CROSS
        "   X    ",
        "   X    ",
        "   X    ",
        "XXXXXXX ",
        "   X    ",
        "   X    ",
        "   X    ",
        "        "
    },
    { // CURSOR_ARROW_BLACK
        "XXXXX   ",
        "XX      ",
        "X X     ",
        "X  X    ",
        "X   X   ",
        "     X  ",
        "      X ",
        "        "
    },
    { // CURSOR_ARROW_WHITE
        ".....   ",
        "..      ",
        ". .     ",
        ".  .    ",
        ".   .   ",
        "     .  ",
        "      . ",
        "        "
    }
};

const ICON util_icons[UTIL_TOTAL] = {
    { // SMILEY
        "  XXXX  ",
        " XYYYYX ",
        "XYXYYXYX",
        "XYYYYYYX",
        "XYXYYXYX",
        "XYYXXYYX",
        " XYYYYX ",
        "  XXXX  "
    },
    { // FROWNY
        "  XXXX  ",
        " XRRRRX ",
        "XRXRRXRX",
        "XRRRRRRX",
        "XRRXXRRX",
        "XRXRRXRX",
        " XRRRRX ",
        "  XXXX  "
    },
    { // HEART
        "        ",
        " RR RR  ",
        "RRRRRRR ",
        "RRRRRRR ",
        " RRRRR  ",
        "  RRR   ",
        "   R    ",
        "        "
    },
    { // FLOPPY
        "XXXXXX  ",
        "X.XX.XX ",
        "X.XX.X.X",
        "X......X",
        "X.XXXX.X",
        "X.XXXX.X",
        "X.XXXX.X",
        "XXXXXXXX"
    },
    { // INFO
        "  XXXX  ",
        " XB..BX ",
        "XBB..BBX",
        "XBBBBBBX",
        "XBB..BBX",
        "XBB..BBX",
        " XB..BX ",
        "  XXXX  "
    },
    { // ERROR
        "  XXXX  ",
        " XRRRRX ",
        "XR.RR.RX",
        "XRR..RRX",
        "XRR..RRX",
        "XR.RR.RX",
        " XRRRRX ",
        "  XXXX  "
    },
    { // GEAR
        "   XX   ",
        " X XX X ",
        "  XGGX  ",
        "XXGXXGXX",
        "XXGXXGXX",
        "  XGGX  ",
        " X XX X ",
        "   XX   "
    }
};

void print_icon(const ICON icon) {
    for (int i = 0; i < 8; i++) {
        printf("%s\r\n", icon[i]);
    }
}

// Draw an icon to the screen at the specified position
void draw_icon(int x, int y, const ICON icon) {
    int icon_width = 8;
    int icon_height = 8;
    int start_x = x;
    int start_y = y;

    for (int row = 0; row < icon_height; row++) {
        for (int col = 0; col < icon_width; col++) {
            int pixel_x = start_x + col;
            int pixel_y = start_y + row;

            switch (icon[row][col]) {
            case '.':
                put_pixel(pixel_x, pixel_y, WHITE_16);
                break;
            case 'X':
                put_pixel(pixel_x, pixel_y, BLACK_16);
                break;
            case 'R':
                put_pixel(pixel_x, pixel_y, RED_16);
                break;
            case 'Y':
                put_pixel(pixel_x, pixel_y, YELLOW_16);
                break;
            case 'B':
                put_pixel(pixel_x, pixel_y, BLUE_16);
                break;
            case 'G':
                put_pixel(pixel_x, pixel_y, LIGHT_GRAY_16);
                break;
            default:
                // Don't draw anything
                break;
            }
        }
    }
}
