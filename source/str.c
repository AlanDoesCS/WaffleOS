//
// Created by Alan on 12/07/2024.
//

#include "str.h"
#include "types.h"

const char* hex_chars = "0123456789ABCDEF";

int strcmp(const char* str1, const char* str2) {
    while (*str1 && *str2) {
        if (*str1 != *str2) {
            return (unsigned char)*str1 - (unsigned char)*str2;
        }
        str1++;
        str2++;
    }

    return (unsigned char)*str1 - (unsigned char)*str2;
}

int int_to_hex_str(uint32_t value, char* buffer) {
    if (!buffer) return -1;
    buffer[8] = '\0';

    for (int i = 7; i >= 0; --i) {
        buffer[i] = hex_chars[value & 0xF];
        value >>= 4;
    }

    return 0;
}