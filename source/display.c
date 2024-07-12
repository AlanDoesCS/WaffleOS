//
// Created by Alan on 11/07/2024.
//

#define VIDEO_MEMORY 0xb8000
#define WHITE_ON_BLACK 0x0f
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

// Helper functions for reading/writing from I/O
extern unsigned char inb(unsigned short port);
extern void outb(unsigned short port, unsigned char val);

int current_row = 0;
int current_col = 0;

void move_cursor(int row, int col) {
    // Calculate the position in the VGA text mode buffer
    int position = (row) * SCREEN_WIDTH + col;

    outb(0x3D4, 0x0F);          // Low cursor control register index
    outb(0x3D5, (unsigned char)(position & 0xFF)); // Low byte of position
    outb(0x3D4, 0x0E);          // High cursor control register index
    outb(0x3D5, (unsigned char)((position >> 8) & 0xFF)); // High byte of position
}

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
        }
    }
    move_cursor(current_row, current_col);
}


void del_last_char() {
	if (current_col == 0) {
		return;
	}

	current_col--;
	print_char_xy('\0', current_col, current_row);
	move_cursor(current_row, current_col);
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