//
// Created by Alan on 12/07/2024.
//

#include "str.h"
#include "types.h"

const char* hex_chars = "0123456789ABCDEF";

int strlen(const char* string) {
    int length = 0;

    if (string == NULL) {
        return 0;
    }

    while (string[length] != '\0') {
        length++;
    }

    return length;
}

char* strcat(char* dest, const char* src) {
    char* ptr = dest;

    while (*ptr != '\0') {
        ptr++;
    }

    while (*src != '\0') {
        *ptr = *src;
        ptr++;
        src++;
    }

    *ptr = '\0';
    return dest;
}

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

char* strcpy(char* dest, const char* src) {
    char* original_dest = dest;

    while (*src != '\0') {
        *dest = *src;
        dest++;
        src++;
    }
    *dest = '\0';

    return original_dest;
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