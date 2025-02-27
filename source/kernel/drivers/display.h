//
// Created by Alan on 11/07/2024.
//

#pragma once

#include "stdint.h"
#include "../types.h"

int get_cursor_col();
int get_cursor_row();
void move_cursor(int row, int col);
void clear();
void print_char_xy(unsigned char c, int col, int row);
void del_last_char();
void print_char(unsigned char c);
void print(const char* str);
void println(const char* str);
void print_uint8(uint8_t value);
void print_uint16(uint16_t value);
void print_uint32(uint32_t value);
void print_hex(uint32_t value, int num_digits);
void print_uint32_hex(uint32_t value);
