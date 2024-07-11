//
// Created by Alan on 11/07/2024.
//

#define VIDEO_MEMORY 0xb8000
#define WHITE_ON_BLACK 0x0f
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

int current_row = 0;
int current_col = 0;

void clear() {
    volatile char* video_memory = (volatile char*)VIDEO_MEMORY;
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        *video_memory++ = ' ';
        *video_memory++ = WHITE_ON_BLACK;
    }

    // go back to start
    current_row = 0;
    current_col = 0;
}


void print_char_xy(unsigned char c, int col, int row) {
    col = col % SCREEN_WIDTH;
    row = row % SCREEN_HEIGHT;

    volatile unsigned char* video_memory = (volatile unsigned char*)VIDEO_MEMORY;
    int offset = 2 * (row * SCREEN_WIDTH + col);
    video_memory[offset] = c;
    video_memory[offset + 1] = WHITE_ON_BLACK;
}


void print_char(unsigned char c) {
    print_char_xy(c, current_col, current_row);
    current_col++;

    if (current_col > SCREEN_WIDTH) {
        current_col=0;
        current_row++;

        if (current_row > SCREEN_HEIGHT) {
            clear();
            current_row = 0;
        }
    }
}


void print(const char* str) {
    for(int i=0; str[i] != 0; i++) {
        print_char(str[i]);
    }
}


void println(const char* str) {
    print(str);
    current_row++;
    current_col = 0;
    if (current_row >= SCREEN_HEIGHT) { // go back to start
        current_row = 0;
    }
}