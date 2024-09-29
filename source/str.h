//
// Created by Alan on 12/07/2024.
//

#ifndef STR_H
#define STR_H

#include "types.h"

int strlen(const char* string);
int isalnum(int c);
int strcmp(const char* str1, const char* str2);
char* strcpy(char* dest, const char* src);
char* strncpy(char* dest, const char* src, size_t n);
char* strcat(char* str1, const char* str2);
char* strchr(const char* str, int character);
char* strpbrk(const char* str, const char* char_set);
int int_to_hex_str(uint32_t value, char* buffer);
int snprintf(char* str, size_t size, const char* format, ...);

#endif //STR_H
