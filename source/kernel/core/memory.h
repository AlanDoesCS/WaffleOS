#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define MEMORY_SIZE 0x100000 // in bytes
#define BLOCK_SIZE 16

typedef struct MemoryBlock {
    size_t Size;
    uint8_t IsFree;
    struct MemoryBlock* Next;
} MemoryBlock;

void init_memory();
void* memset(void* dst0, int value, size_t length);
void* memcpy(void* dst0, const void* src0, size_t length);
int memcmp(const void* ptr1, const void* ptr2, uint16_t num);
void* malloc(size_t size);
void* calloc(size_t num, size_t size);
void free(void* ptr);
