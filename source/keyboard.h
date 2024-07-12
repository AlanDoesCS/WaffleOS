//
// Created by Alan on 11/07/2024.
//

#ifndef KEYBOARD_H
#define KEYBOARD_H

unsigned char inb(unsigned short port);
void outb(unsigned short port, unsigned char val);
void reset_keyboard(void);
void idt_init(void);
void init_keyboard(void);
char* read_line(void);

#endif //KEYBOARD_H
