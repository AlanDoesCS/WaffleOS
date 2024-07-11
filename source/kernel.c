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

void kernel_main(void) {
    char *video_memory = (char *)0xb8000;
    const char *message = "Welcome to WaffleOS!";
    int i = 0;

    while (message[i] != 0) {
        video_memory[i * 2] = message[i];
        video_memory[i * 2 + 1] = 0x07; // Light grey on black background
        i++;
    }

    while (1) { }
}