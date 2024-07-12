//
// Created by Alan on 12/07/2024.
//

#include "disk.h"
#include "display.h"

// Helper functions for reading/writing from I/O
extern unsigned char inb(unsigned short port);
extern void outb(unsigned short port, unsigned char val);

void init_disk(void) {
    println("[DISK] Disk initialized");
}