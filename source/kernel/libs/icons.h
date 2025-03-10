#pragma once

typedef char ICON[8][8]; // 8x8 icon type

typedef enum {
    CURSOR_DEFAULT,
    CURSOR_HAND,
    CURSOR_CROSS,
    CURSOR_ARROW_BLACK,
    CURSOR_ARROW_WHITE,
    CURSOR_TOTAL
} CursorType;

extern const ICON cursor_shapes[CURSOR_TOTAL];

// Utility icons
typedef enum {
    SMILEY,
    FROWNY,
    HEART,
    FLOPPY,
    INFO,
    ERROR,
    GEAR,
    UTIL_TOTAL
} UtilIconType;

extern const ICON util_icons[UTIL_TOTAL];

void print_icon(const ICON icon);
void draw_icon(int x, int y, const ICON icon);
