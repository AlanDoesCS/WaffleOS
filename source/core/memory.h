#ifndef MEMORY_H
#define MEMORY_H

#include "../types.h"

#define MEMORY_SIZE 0x100000    // 1MB
#define BLOCK_SIZE 16


typedef struct MemoryBlock {
    size_t Size;
    uint8_t IsFree;
    struct MemoryBlock* Next;
} MemoryBlock;

void init_memory(void);
void* memset(void* dst0, int value, size_t length);
void* memcpy(void* dst0, const void* src0, size_t length);
void* malloc(size_t size);
void* calloc(size_t num, size_t size);
void free(void* ptr);

#endif //MEMORY_H
