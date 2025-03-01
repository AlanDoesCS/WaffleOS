#include <stdint.h>
#include <stddef.h>
#include "stdio.h"
#include "memory.h"

#include "../drivers/display.h"
#include "../libs/string.h"
#include "kernel.h"

extern uint8_t __bss_start;
extern uint8_t __end;

typedef struct BootInfo {
    uint16_t boot_drive;
    uint32_t lfb_address;
} BootInfo;

//void __attribute__((section(".entry"))) start(BootInfo *boot_info) {
void __attribute__((section(".entry"))) start(uint16_t boot_drive) {
    memset((void*)(&__bss_start), (int)0, (size_t)((&__end) - (&__bss_start)));
    //display_init(boot_info->lfb_address, SCREEN_WIDTH * BYTES_PER_PIXEL);
    //g_clrscr(0x55555555);  // Clear the screen.

    clrscr();
	print_splash();

    init_memory();
    init_idt();
    init_pit();
    init_disk();
    //init_filesystem();    // Currently non-functional
	init_keyboard();

    enable_interrupts();

    printf("[KERNEL] Kernel initialisation complete\r\n");
    while(1) {
        printf("root $ ");
        char* input = read_line();

        execute_command(input);
    }

end: // should be unreachable
    for (;;);
}

void execute_command(char* command) {
    if (strcmp(command, "clear") == 0) {
        clrscr();
        return;
    } else if (strcmp(command, "help") == 0) {
        printf("\r\nAvailable commands:\r\nclear - Clear the screen\r\nhelp - Display this help message\r\nsystime - Prints the time since startup\r\n");
        return;
    } else if (strcmp(command, "shutdown") == 0) { // TODO: Implement shutdown
        printf("Shutting down...\r\n");
    } else if (strcmp(command, "systime") == 0) {
        printf("Time since startup: ");
        print_systime();
        printf("\r\n");
    } else if (strcmp(command, "hello") == 0) {
        printf("Hello, World!\r\n");
    } else if ((strcmp(command, "waffle") == 0) | (strcmp(command, "waffleos") == 0)) {
        print_splash();
        printf(" _________________\r\n");
        printf("/ ._____________. \\\r\n");
        printf("| |_|_|_|_|_|_|_| |\r\n");
        printf("| |_|_|_|_|_|_|_| |\r\n");
        printf("| |_|_|_|_|_|_|_| |\r\n");
        printf("| |_|_|_|_|_|_|_| |\r\n");
        printf("| |_|_|_|_|_|_|_| |\r\n");
        printf("| |_|_|_|_|_|_|_| |\r\n");
        printf("\\_________________/\r\n\r\n");
    } else if (strcmp(command, "cowsay") == 0) {
        cowsay("WaffleOS!");
    } else if (strcmp(command, "enablegraphics") == 0) {
        // Enable graphics mode
        enable_graphics();

        g_clrscr(0x0000FF00);  // Clear the screen.

        while(true) {
            continue;
        }
        //printf("Made it back safe\r\n");

        /*
        draw_rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0x00000000);

        // Draw a white line from (100, 100) to (200, 200).
        draw_line(100, 100, 200, 200, 0x00FFFFFF);

        // Draw a red rectangle at (50, 50) of size 100x80.
        draw_rect(50, 50, 100, 80, 0x00FF0000);

        // Draw the character 'A' in green at (120, 120).
        draw_char(120, 120, 'A', 0x0000FF00);

        // Draw a string.
        draw_string(50, 150, "Hello, World!\nNew line", 0x00FFFFFF);
         */
    } else {
        // Default case: Print the entered command
        printf("Unrecognised command: %s\r\n", command);
    }
}

void print_splash(void) {
    printf("\n\t  \xC9\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xBB\r\n");  // ╔══════════╗
    printf("\t  \xBA WaffleOS \xBA\r\n");                                  // ║ WaffleOS ║
    printf("\t  \xC8\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xBC\n\r\n");  // ╚══════════╝
}

void cowsay(char* message) {    // TODO: change once string concatenation is improved (requires memory allocation)
    printf(" ___________\r\n< %s >\r\n -----------\r\n        \\   ^__^\r\n         \\  (oo)\\_______\r\n            (__)\\       )\\/\\\r\n                ||----w |\r\n                ||     ||\r\n\r\n", message);;
}