#include "display.h"
#include "idt.h"
#include "keyboard.h"
#include "str.h"
#include "kernel.h"
#include "disk.h"
#include "timer.h"

void kernel_main(void) __attribute__((section(".text.kernel_entry")));

void kernel_main(void) {
    clear();
	print_splash();

    init_idt();
    init_disk();
	init_keyboard();
    init_pit();

    enable_interrupts();

    println("[KERNEL] Kernel initialisation complete");
    println("[PIT] Programmable Interval Timer is functioning");
	println("");
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
        println("");
    } else if (strcmp(command, "cowsay") == 0) {
        cowsay("WaffleOS!");
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

void cowsay(char* message) {    // TODO: change once string concatenation is improved (requires memory allocation)
    println(" ___________");
    print("< ");
    print(message);
    println(" >");
    println(" -----------");
    println("        \\   ^__^");
    println("         \\  (oo)\\_______");
    println("            (__)\\       )\\/\\");
    println("                ||----w |");
    println("                ||     ||");
    println("");
}