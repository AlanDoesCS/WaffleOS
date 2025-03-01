/* Derived from: https://embeddedartistry.com/blog/2017/03/22/memset-memcpy-memcmp-and-memmove/
and: https://opensource.apple.com/source/xnu/xnu-2050.7.9/libsyscall/wrappers/memcpy.c

malloc(), calloc() and free() were based on a simplified implementation of a combination of:
- https://arjunsreedharan.org/post/148675821737/memory-allocators-101-write-a-simple-memory
- https://embeddedartistry.com/blog/2017/02/15/implementing-malloc-first-fit-free-list/
- logic from: https://pages.cs.wisc.edu/~remzi/OSTEP/vm-api.pdf
*/

#include "memory.h"

#include <stdint.h>
#include <stddef.h>

#include "stdio.h"
#include "../libs/string.h"

#define wsize sizeof(uint32_t)
#define wmask (wsize - 1)

static uint32_t heap[MEMORY_SIZE];    // 1MB
static MemoryBlock* head = NULL;

void merge_surrounding_free_blocks(MemoryBlock* block) {
    MemoryBlock* current = head;
    MemoryBlock* previous = NULL;

    while (current && current != block) {    // Loop until we find the block, or reach the end of the linked list
        previous = current;
        current = current->Next;
    }

    if (!current) {    // Block not in linked list
        return;
    }

    // merge with previous block if free
    if (previous && previous->IsFree) {
        previous->Size += sizeof(MemoryBlock) + block->Size;
        previous->Next = block->Next;
        current = previous;
    }

    // merge with all subsequent blocks if free
    while (current->Next && current->Next->IsFree) {
        current->Size += sizeof(MemoryBlock) + current->Next->Size;
        current->Next = current->Next->Next;
    }
}

// NOTE: I DID NOT WRITE THIS, ALL CREDIT GOES TO PHILLIP JOHNSTON
// From: https://embeddedartistry.com/blog/2017/03/22/memset-memcpy-memcmp-and-memmove/
void* memset(void* dst0, int value, size_t length) {
    unsigned char *s = dst0;
    uint32_t c = (unsigned char)value;
    size_t n = length;
    size_t k;

    /* Fill head and tail with minimal branching. Each
     * conditional ensures that all the subsequently used
     * offsets are well-defined and in the dst0 region. */

    if (!n) return dst0;
    s[0] = s[n-1] = c;
    if (n <= 2) return dst0;
    s[1] = s[n-2] = c;
    s[2] = s[n-3] = c;
    if (n <= 6) return dst0;
    s[3] = s[n-4] = c;
    if (n <= 8) return dst0;

    /* Advance pointer to align it at a 4-byte boundary,
     * and truncate n to a multiple of 4. The previous code
     * already took care of any head/tail that get cut off
     * by the alignment. */

    k = -(uintptr_t)s & 3;
    s += k;
    n -= k;
    n &= -4;
    n /= 4;

    uint32_t *ws = (uint32_t *)s;
    uint32_t wc = c & 0xFF;
    wc |= ((wc << 8) | (wc << 16) | (wc << 24));

    /* Pure C fallback with no aliasing violations. */
    for (; n; n--, ws++) *ws = wc;

    return dst0;
}

// NOTE: I DID NOT WRITE THIS, ALL CREDIT GOES TO APPLE INC.
// From: https://opensource.apple.com/source/xnu/xnu-2050.7.9/libsyscall/wrappers/memcpy.c
void* memcpy(void* dst0, const void* src0, size_t length) {
    char* dst = dst0;
    const char* src = src0;
    size_t t;

    if (length == 0 || dst == src)		/* nothing to do */
        goto done;

    /*
     * Macros: loop-t-times; and loop-t-times, t>0
     */
#define	TLOOP(s) if (t) TLOOP1(s)
#define	TLOOP1(s) do { s; } while (--t)

    if ((uint32_t)dst < (uint32_t)src) {
        /*
         * Copy forward.
         */
        t = (uintptr_t)src;	/* only need low bits */
        if ((t | (uintptr_t)dst) & wmask) {
            /*
             * Try to align operands.  This cannot be done
             * unless the low bits match.
             */
            if ((t ^ (uintptr_t)dst) & wmask || length < wsize)
                t = length;
            else
                t = wsize - (t & wmask);
            length -= t;
            TLOOP1(*dst++ = *src++);
        }
        /*
         * Copy whole words, then mop up any trailing bytes.
         */
        t = length / wsize;
        TLOOP(*(uint32_t*)dst = *(uint32_t*)src; src += wsize; dst += wsize);
        t = length & wmask;
        TLOOP(*dst++ = *src++);
    } else {
        /*
         * Copy backwards.  Otherwise essentially the same.
         * Alignment works as before, except that it takes
         * (t&wmask) bytes to align, not wsize-(t&wmask).
         */
        src += length;
        dst += length;
        t = (uintptr_t)src;
        if ((t | (uintptr_t)dst) & wmask) {
            if ((t ^ (uintptr_t)dst) & wmask || length <= wsize)
                t = length;
            else
                t &= wmask;
            length -= t;
            TLOOP1(*--dst = *--src);
        }
        t = length / wsize;
        TLOOP(src -= wsize; dst -= wsize; *(uint32_t*)dst = *(uint32_t*)src);
        t = length & wmask;
        TLOOP(*--dst = *--src);
    }
    done:
        return (dst0);
}

void* malloc(size_t size) {
    MemoryBlock *current, *previous;
    void* result;

    if (!head) {
        init_memory();
    }

    current = head;
    while (current) {
        if (current->IsFree && current->Size >= size) {    // If the block is free and large enough
            if (current->Size > size + sizeof(MemoryBlock) + BLOCK_SIZE) {    // If the block is larger than the requested size
                MemoryBlock* new_block = (MemoryBlock*)((uint8_t*)current + sizeof(MemoryBlock) + size);
                new_block->Size = current->Size - size - sizeof(MemoryBlock);
                new_block->IsFree = true;
                new_block->Next = current->Next;
                current->Size = size;
                current->Next = new_block;
            }
            current->IsFree = false;
            result = (void*)(current + 1);
            return result;
        }
        previous = current;
        current = current->Next;
    }

    return NULL;
}

void* calloc(size_t num_items, size_t size) {
    size_t total_size = num_items * size;
    void* result = malloc(total_size);
    if (result) {
        memset(result, 0, total_size);
    }
    return result;
}

void free(void *ptr) {
    if (!ptr) {
        return;
    }

    MemoryBlock* block = (MemoryBlock*)ptr - 1;
    block->IsFree = true;

    merge_surrounding_free_blocks(block);
}

// for debug only! (inclusive, not inclusive)
void memdump_array(uint8_t* address, int start, int end) {
    for (int i = start; i < end; i += 16) {
        // print offset
        printf("%x: ", i);

        // Print 16 bytes on this line
        for (int j = i; j < i + 16 && j < end; j++) {
            // Manually add a leading zero if the value is less than 16
            if (address[j] < 16) {
                printf("0%x ", address[j]);
            } else {
                printf("%x ", address[j]);
            }
        }
        printf("\r\n");
    }
}

void init_memory() {
    head = (MemoryBlock*)heap;
    head->Size = MEMORY_SIZE - sizeof(MemoryBlock);
    head->IsFree = true;
    head->Next = NULL;
    printf("[MEMORY] Heap initialised\r\n");
}