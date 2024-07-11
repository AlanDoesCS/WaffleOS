#include "display.h"

void kernel_main(void) __attribute__((section(".text.kernel_entry")));

void kernel_main(void) {
    clear();
    println("[KERNEL] Kernel loaded successfully");
    println("");
    println("            \xC9\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xBB");  // ╔══════════╗
    println("            \xBA WaffleOS \xBA");                                // ║ WaffleOS ║
    println("            \xC8\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xBC");  // ╚══════════╝

    while(1);
}
