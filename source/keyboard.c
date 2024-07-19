//
// Created by Alan on 11/07/2024.
//

#include "keyboard.h"
#include "display.h"
#include "idt.h"
#include "kernel.h"
#include "types.h"

// Helper functions for reading/writing from I/O
extern unsigned char inb(unsigned short port);
extern void outb(unsigned short port, unsigned char val);

extern void irq1(void);

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define BACKSPACE_SCANCODE 0x0E
#define ENTER_SCANCODE 0x1C
#define MAX_INPUT_LENGTH 256

char input_buffer[MAX_INPUT_LENGTH];
int buffer_index = 0;
volatile int line_ready = 0;  // line ready to return flag
volatile int min_col = 0;

// US keyboard layout mapping
const char scancode_to_ascii[] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' '
};

void keyboard_handler(void) {
    unsigned char status;
    unsigned char scancode;
    char ascii;

    status = inb(KEYBOARD_STATUS_PORT);
    if (status & 0x01) {
        scancode = inb(KEYBOARD_DATA_PORT);

        if (!(scancode & 0x80)) {    // ensure key press
            if (scancode == ENTER_SCANCODE) {
                print_char('\n');
                input_buffer[buffer_index] = '\0';
                line_ready = 1;  // set flag
                send_eoi(1);  // send end of interrupt signal
                return;
            } else if (scancode == BACKSPACE_SCANCODE) { // backspace
                if (buffer_index == 0) {
                    send_eoi(1);  // send end of interrupt signal
                    return;
                }
                buffer_index--;
                input_buffer[buffer_index] = '\0';
                del_last_char();
            } else if (buffer_index < MAX_INPUT_LENGTH - 1) {
                ascii = scancode_to_ascii[scancode];
                if (ascii != 0) {
                    input_buffer[buffer_index++] = ascii;
                    print_char(ascii);
                }
            }
        }
    }

    send_eoi(1);  // send end of interrupt signal
}

void init_keyboard(void) {
    println("[I/O] Initializing keyboard...");

    register_interrupt_handler(33, (uint32_t)irq1);

    // enable keyboard IRQ
    enable_irq(1);

    println("[I/O] Keyboard initialized");
}


void reset_keyboard(void) {
    buffer_index = 0;
    line_ready = 0;
    min_col = get_cursor_col();
}


char* read_line(void) {
    reset_keyboard();
    while (!line_ready) {
        __asm__("hlt");  // halt CPU until next interrupt
    }
    return input_buffer;
}