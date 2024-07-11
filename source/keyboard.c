//
// Created by Alan on 11/07/2024.
//

#include "keyboard.h"
#include "display.h"
#include "idt.h"

#define KEYBOARD_DATA_PORT 0x60
#define ENTER_KEY 0x1C
#define BACKSPACE_KEY 0x0E
#define MAX_INPUT_LENGTH 256

char input_buffer[MAX_INPUT_LENGTH];
int buffer_index = 0;

// US keyboard layout mapping
const char scancode_to_ascii[] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' '
};

void keyboard_handler(void) {
    unsigned char scancode;
    char ascii;

    scancode = inb(KEYBOARD_DATA_PORT);

    if (scancode & 0x80) { // ignore key release
        return;
    }

    if (scancode == ENTER_KEY) {
        input_buffer[buffer_index] = '\0';
        println("");
        buffer_index = 0;
        return;
    }

    if (scancode == BACKSPACE_KEY && buffer_index > 0) {
        buffer_index--;
        print("\b \b");
        return;
    }

    if (buffer_index >= MAX_INPUT_LENGTH - 1) { //ignore input after limit is reached
        return;
    }

    ascii = scancode_to_ascii[scancode];
    if (ascii != 0) {
        input_buffer[buffer_index++] = ascii;
        print_char(ascii);  // Print the character
    }
}

void init_keyboard(void) {
    init_idt();  // Initialize the IDT

    // Remap the PIC
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);
    outb(0xA1, 0x0);

    // Enable keyboard IRQ
    outb(0x21, 0xFD);
}

char* read_line(void) {
    buffer_index = 0;
    while (input_buffer[buffer_index - 1] != '\n') {
        // Wait for Enter key
        __asm__("hlt");  // Halt CPU until next interrupt
    }
    input_buffer[buffer_index - 1] = '\0';  // Replace newline with null terminator
    return input_buffer;
}

// Helper functions for reading/writing from I/O
unsigned char inb(unsigned short port) {
    unsigned char value;
    __asm__ __volatile__("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

void outb(unsigned short port, unsigned char val) {
    __asm__ __volatile__("outb %0, %1" : : "a"(val), "Nd"(port));
}