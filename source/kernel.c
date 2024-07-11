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

void clear_screen() {
    unsigned char *video_memory = (unsigned char *)0xb8000;
    for (int i = 0; i < 80 * 25; i++) {
        video_memory[i * 2] = ' ';
        video_memory[i * 2 + 1] = 0x07;
    }
}

void print(const char* str) {
    volatile char* video_memory = (volatile char*)VIDEO_MEMORY;
    while(*str != 0) {
        *video_memory++ = *str++;
        *video_memory++ = WHITE_ON_BLACK;
    }
}

void kernel_main(void) {
    clear_screen();
    print("Kernel loaded successfully!");
    while(1);
}
