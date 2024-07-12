#include "display.h"
#include "keyboard.h"
#include "str.h"
#include "kernel.h"
#include "disk.h"

void kernel_main(void) __attribute__((section(".text.kernel_entry")));

void kernel_main(void) {
    clear();
    println("[KERNEL] Kernel loaded successfully");

    init_disk();
	init_keyboard();

	print_splash();

    while(1) {
        print("root $ ");
        char* input = read_line();

        execute_command(input);
    }
}

void execute_command(char* command) {
    if (strcmp(command, "clear") == 0) {
        clear();
        return;
    } else if (strcmp(command, "help") == 0) {
        println("");
        println("Available commands:");
        println("  clear - Clear the screen");
        println("  help - Display this help message");
        println("");
        return;
    } else if (strcmp(command, "shutdown") == 0) { // TODO: Implement shutdown
        println("Shutting down...");
    } else if (strcmp(command, "hello") == 0) {
        println("Hello, World!");
    } else if ((strcmp(command, "waffle") == 0) | (strcmp(command, "waffleos") == 0)) {
        print_splash();
        println(" _________________");
        println("/ ._____________. \\");
        println("| |_|_|_|_|_|_|_| |");
        println("| |_|_|_|_|_|_|_| |");
        println("| |_|_|_|_|_|_|_| |");
        println("| |_|_|_|_|_|_|_| |");
        println("| |_|_|_|_|_|_|_| |");
        println("| |_|_|_|_|_|_|_| |");
        println("\\_________________/");
    } else {
        // Default case: Print the entered command
        print("Unrecognised command: ");
        println(command);
        println("");
    }
}

void print_splash(void) {
    println("");
    println("                                  \xC9\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xBB");  // ╔══════════╗
    println("                                  \xBA WaffleOS \xBA");                                // ║ WaffleOS ║
    println("                                  \xC8\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xBC");  // ╚══════════╝
    println("");
}