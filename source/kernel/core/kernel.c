#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <float.h>
#include "stdio.h"
#include "memory.h"

#include "../drivers/display.h"
#include "../drivers/floppy.h"
#include "../drivers/fat.h"
#include "../libs/string.h"
#include "../libs/math.h"
#include "kernel.h"

extern uint8_t __bss_start;
extern uint8_t __end;

void __attribute__((section(".entry"))) start(uint16_t boot_drive) {
    memset((void*)(&__bss_start), (int)0, (size_t)((&__end) - (&__bss_start)));

    clrscr();
    print_splash();

    init_memory();
    init_idt();
    init_pit();
    init_disk();
    //init_filesystem();    // Currently non-functional
    init_keyboard();
    init_fpu();

    enable_interrupts();

    printf("[KERNEL] Kernel initialisation complete\r\n");
    execute_command("enablegraphics");
    while(true) {
        printf("root $ ");
        char* input = read_line();

        execute_command(input);
    }

    end: // should be unreachable
        for (;;);
}

void execute_command(char* command) {
    SinglyLinkedList* args = strsplit(command, ' ');
    char* cmd = NULL;
    char** cmd_ptr = (char**)SinglyLinkedList_Get(args, 0);
    if (cmd_ptr) {
        cmd = *cmd_ptr;
    }

    if (strcmp(cmd, "clear") == 0) {
        clrscr();
        return;
    } else if (strcmp(cmd, "help") == 0) {
        printf("\r\nAvailable commands:\r\nclear - Clear the screen\r\nhelp - Display this help message\r\nsystime - Prints the time since startup\r\n");
        return;
    } else if (strcmp(cmd, "shutdown") == 0) { // TODO: Implement shutdown
        printf("Shutting down...\r\n");
    } else if (strcmp(cmd, "systime") == 0) {
        printf("Time since startup: ");
        print_systime();
        printf("\r\n");
    } else if (strcmp(cmd, "hello") == 0) {
        printf("Hello, World!\r\n");
    } else if ((strcmp(cmd, "waffle") == 0) || (strcmp(command, "waffleos") == 0)) {
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
    } else if (strcmp(cmd, "cowsay") == 0) {
        char* message = "WaffleOS!";
        char** message_ptr = (char**)SinglyLinkedList_Get(args, 1);
        if (message_ptr) {
            message = *message_ptr;
        }
        cowsay(message);
    } else if (strcmp(cmd, "mkdir") == 0) {
        char* dir = NULL;
        char** dir_ptr = (char**)SinglyLinkedList_Get(args, 1);
        if (dir_ptr) {
            dir = *dir_ptr;
        }
        FAT_MakeDirectory(dir);
    } else if (strcmp(cmd, "cd") == 0) {
    } else if (strcmp(cmd, "touch") == 0) {
    } else if (strcmp(cmd, "cat") == 0) {
    } else if (strcmp(cmd, "mv") == 0) {
    } else if (strcmp(cmd, "ls") == 0) {
    } else if (strcmp(cmd, "vis") == 0) {
        printf("Starting visualiser for \r\n");
    } else if (strcmp(cmd, "ping") == 0) {
        char* target = NULL;
        char** target_ptr = (char**)SinglyLinkedList_Get(args, 1);
        if (target_ptr) {
            target = *target_ptr;
        }
        printf("PING %s: 56 data bytes\r\n", target);
        printf("--- %s ping statistics ---\r\n", target);
        printf("2 packets transmitted, 2 received, 0%% packet loss, time 1001ms\r\n");
    } else if (strcmp(cmd, "enablegraphics") == 0) {
        char* mode = NULL;
        char** mode_ptr = (char**)SinglyLinkedList_Get(args, 1);
        if (mode_ptr) {
            mode = *mode_ptr;
        }
        enable_graphics_mode(*mode - '0');
        g_clear_screen();

        draw_rect(0, 0, 319, 199, LIGHT_BLUE_16);
        draw_rect(0, 0, 319, 20, BLUE_16);
        draw_line(0, 20, 319, 199, RED_16);
        draw_line(319, 20, 0, 199, RED_16);
        draw_string(3, 3, "Welcome to WaffleOS!", YELLOW_16);
        for (int i = 1; i < 10; i++) {
            draw_scaled_string(10, 20 + (i * 20), "Lorem ipsum dolor sit amet", RED_16, 1.0f-(0.1f*(i-1)));
        }

        while(true);
    } else {
        printf("Unrecognised command: %s\r\n", cmd);
    }

    SinglyLinkedList_Free(args);
}

void print_splash(void) {
    printf("\n\t  \xC9\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xBB\r\n");  // ╔══════════╗
    printf("\t  \xBA WaffleOS \xBA\r\n");                                  // ║ WaffleOS ║
    printf("\t  \xC8\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xBC\n\r\n");  // ╚══════════╝
}

void cowsay(char* message) {    // TODO: change once string concatenation is improved (requires memory allocation)
    size_t len = strlen(message);
    size_t width = len + 2; // add padding

    printf(" ");
    for (size_t i = 0; i < width; i++) {
        printf("_");
    }
    printf("\n");

    printf("< %s >\n", message);

    printf(" ");
    for (size_t i = 0; i < width; i++) {
        printf("-");
    }
    printf("\n");

    printf("        \\   ^__^\r\n         \\  (oo)\\_______\r\n            (__)\\       )\\/\\\r\n                ||----w |\r\n                ||     ||\r\n\r\n", message);;
}