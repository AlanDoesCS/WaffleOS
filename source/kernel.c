// Remember to use the -nostdlib flag when compiling C source files!

/*
BOX CHARACTERS (TBA)
╔ = 0xC9
╗ = 0xBB
╚ = 0xC8
╝ = 0xBC
═ = 0xCD
║ = 0xBA
*/

void kernel_main(void) __attribute__((section(".text.kernel_entry")));

#define VIDEO_MEMORY 0xb8000
#define WHITE_ON_BLACK 0x0f
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

int current_row = 0;

void clear_screen() {
    volatile char* video_memory = (volatile char*)VIDEO_MEMORY;
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        *video_memory++ = ' ';
        *video_memory++ = WHITE_ON_BLACK;
    }

    current_row = 0; // go back to start
}

void print(const char* str, int row) {
    volatile char* video_memory = (volatile char*)VIDEO_MEMORY;
    int offset = 2 * SCREEN_WIDTH * row;
    video_memory += offset;
    while(*str != 0) {
        *video_memory++ = *str++;
        *video_memory++ = WHITE_ON_BLACK;
    }
}

void println(const char* str) {
    print(str, current_row);
    current_row++;
    if (current_row >= SCREEN_HEIGHT) { // go back to start
        current_row = 0;
    }
}

void kernel_main(void) {
    clear_screen();
    println("[KERNEL] Kernel loaded successfully");
    println("");
    println("╔══════════╗");
    println("║ WaffleOS ║");
    println("╚══════════╝");

    while(1);
}
