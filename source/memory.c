//
// Created by Alan on 18/07/2024.
// Derived from: https://embeddedartistry.com/blog/2017/03/22/memset-memcpy-memcmp-and-memmove/
// and: https://opensource.apple.com/source/xnu/xnu-2050.7.9/libsyscall/wrappers/memcpy.c

#include "memory.h"
#include "types.h"

#define wsize sizeof(uint32_t)
#define wmask (wsize - 1)

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