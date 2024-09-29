#include "display.h"
#include "idt.h"
#include "keyboard.h"
#include "str.h"
#include "filesystem.h"
#include "kernel.h"
#include "disk.h"
#include "timer.h"
#include "memory.h"

void kernel_main(void) __attribute__((section(".text.kernel_entry")));

void kernel_main(void) {
    clear();
	print_splash();

    //init_memory();
    //init_idt();
    //init_pit();
    //init_disk();
    //init_filesystem();    // Currently non-functional
	//init_keyboard();

    //enable_interrupts();

    println("[KERNEL] Kernel initialisation complete\n");
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
        println("  systime - Prints the time since startup");
        println("");
        return;
    } else if (strcmp(command, "shutdown") == 0) { // TODO: Implement shutdown
        println("Shutting down...");
    } else if (strcmp(command, "systime") == 0) { // TODO: Implement shutdown
        print("Time since startup: ");
        print_systime();
        print_char('\n');
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
    }
}

void print_splash(void) {
    println("\n\t\t\t\t\t\t\t\t  \xC9\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xBB");  // ╔══════════╗
    println("\t\t\t\t\t\t\t\t  \xBA WaffleOS \xBA");                                  // ║ WaffleOS ║
    println("\t\t\t\t\t\t\t\t  \xC8\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xBC\n");  // ╚══════════╝
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