// strlen implementation from: https://github.com/nanobyte-dev/nanobyte_os/blob/videos/part7/src/bootloader/stage2/string.c
#include "string.h"
#include <stdint.h>
#include <stddef.h>
#include "../core/stdio.h"

#include "../core/memory.h"
#include "datastructures.h"

// Nanobyte's code starts here

unsigned strlen(const char* str)
{
    unsigned len = 0;
    while (*str)
    {
        ++len;
        ++str;
    }

    return len;
}

// Nanobyte's code ends here

const char* hex_chars = "0123456789ABCDEF";

int isalnum(int c) {
    return ((c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z') ||
            (c >= '0' && c <= '9'));
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

char* strncpy(char* dest, const char* src, size_t n) {
    size_t i;

    for (i = 0; i < n && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }

    for (; i < n; i++) {
        dest[i] = '\0';
    }

    return dest;
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

char* strchr(const char* str, int character) {
    while (*str != '\0') {
        if (*str == (char)character) {
            return (char*)str;
        }
        str++;
    }
    return NULL;
}

char* strpbrk(const char* str, const char* char_set) {
    while (*str != '\0') {
        const char* set = char_set;
        while (*set != '\0') {
            if (*str == *set) {
                return (char*)str;
            }
            set++;
        }
        str++;
    }
    return NULL;
}

SinglyLinkedList* strsplit(char* src, char delimiter) {
    if (src == NULL) return NULL;  // Check for NULL

    // Create tokens list
    SinglyLinkedList* list = SinglyLinkedList_Create(sizeof(char*));
    if (list == NULL) return NULL;  // failure

    char* token_start = src;
    for (char* p = src; ; p++) {
        if (*p == delimiter || *p == '\0') {
            size_t token_length = p - token_start;
            // Allocate memory for the token and null terminator
            char* token = malloc(token_length + 1);
            if (!token) {
                return NULL;
            }
            memcpy(token, token_start, token_length);
            token[token_length] = '\0';

            // Append the token to the list
            SinglyLinkedList_Append(list, &token);

            if (*p == '\0') break;
            token_start = p + 1;
        }
    }
    return list;
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

void int_to_str(int num, char* str, int* len) {
    int i = 0;
    int is_negative = 0;

    if (num == 0) {
        str[i++] = '0';
        *len = i;
        return;
    }

    if (num < 0) {
        is_negative = 1;
        num = -num;
    }

    while (num != 0) {
        int rem = num % 10;
        str[i++] = rem + '0';
        num = num / 10;
    }

    if (is_negative) {
        str[i++] = '-';
    }

    *len = i;

    for (int j = 0; j < i / 2; j++) {
        char temp = str[j];
        str[j] = str[i - 1 - j];
        str[i - 1 - j] = temp;
    }
}

bool islower(char chr)
{
    return chr >= 'a' && chr <= 'z';
}

char toupper(char chr)
{
    return islower(chr) ? (chr - 'a' + 'A') : chr;
}
