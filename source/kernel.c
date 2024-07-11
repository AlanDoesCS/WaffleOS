#include "display.h"
#include "keyboard.h"

void kernel_main(void) __attribute__((section(".text.kernel_entry")));

void kernel_main(void) {
    clear();
    println("[KERNEL] Kernel loaded successfully");
    println("");
    println("                \xC9\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xBB");  // ╔══════════╗
    println("                \xBA WaffleOS \xBA");                                // ║ WaffleOS ║
    println("                \xC8\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xBC");  // ╚══════════╝

    while(1) {
        print("Enter a command: ");
        char* input = read_line();
        println("");
        print("You entered: ");
        println(input);
    }
}
