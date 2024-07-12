#include "display.h"
#include "keyboard.h"
#include "str.h"
#include "kernel.h"

void kernel_main(void) __attribute__((section(".text.kernel_entry")));

extern volatile int line_ready;

void kernel_main(void) {
    clear();
    println("[KERNEL] Kernel loaded successfully");

	init_keyboard();

	println("");
    println("                                  \xC9\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xBB");  // ╔══════════╗
    println("                                  \xBA WaffleOS \xBA");                                // ║ WaffleOS ║
    println("                                  \xC8\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xBC");  // ╚══════════╝


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
        println("Available commands:");
        println("  clear - Clear the screen");
        println("  help - Display this help message");
        println("");
        return;
    }

    if (strcmp(command, "hello") == 0) {
        println("Hello, World!");
    } else {
        // Default case: Print the entered command
        print("Unrecognised command: ");
        println(command);
        println("");
    }
}
