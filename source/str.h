//
// Created by Alan on 12/07/2024.
//

#ifndef STR_H
#define STR_H

#include "types.h"

int strlen(const char* string);
int strcmp(const char* str1, const char* str2);
char* strcpy(char* dest, const char* src);
char* strcat(char* str1, const char* str2);
int int_to_hex_str(uint32_t value, char* buffer);

#endif //STR_H
