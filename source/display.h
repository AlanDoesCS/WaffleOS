//
// Created by Alan on 11/07/2024.
//

#ifndef DISPLAY_H
#define DISPLAY_H

void clear();
void print_char_xy(unsigned char c, int col, int row);
void print_char(unsigned char c);
void print(const char* str);
void println(const char* str);

#endif //DISPLAY_H
