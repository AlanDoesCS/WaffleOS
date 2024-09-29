//
// Created by Alan on 12/07/2024.
//

#include "str.h"
#include "memory.h"
#include "types.h"
#include "datastructures.h"

const char* hex_chars = "0123456789ABCDEF";

int isalnum(int c) {
    return ((c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z') ||
            (c >= '0' && c <= '9'));
}

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

char* split(char* src, char delimiter) {
  char* ptr = src;
  int count = 0;

  SinglyLinkedList* list = SinglyLinkedList_Create(sizeof(char*));
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

static void int_to_str(int num, char* str, int* len) {
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

int snprintf(char* str, size_t size, const char* format, ...) {
    if (size == 0) return 0;

    size_t written = 0;
    size_t max_size = size - 1;  // Reserve space for null terminator
    char* current = str;
    const char* fmt = format;

    __builtin_va_list args;
    __builtin_va_start(args, format);

    while (*fmt && written < max_size) {
        if (*fmt != '%') {
            *current++ = *fmt++;
            written++;
        } else {
            fmt++;
            switch (*fmt) {
            case 's': {
                    const char* s = __builtin_va_arg(args, const char*);
                    size_t len = strlen(s);
                    if (written + len > max_size) {
                        len = max_size - written;
                    }
                    memcpy(current, s, len);
                    current += len;
                    written += len;
                    break;
            }
            case 'd': {
                    int d = __builtin_va_arg(args, int);
                    char num_str[20];
                    int num_len;
                    int_to_str(d, num_str, &num_len);
                    if (written + num_len > max_size) {
                        num_len = max_size - written;
                    }
                    memcpy(current, num_str, num_len);
                    current += num_len;
                    written += num_len;
                    break;
            }
            default:
                *current++ = *fmt;
                written++;
                break;
            }
            fmt++;
        }
    }

    *current = '\0';
    __builtin_va_end(args);

    return (int)written;
}