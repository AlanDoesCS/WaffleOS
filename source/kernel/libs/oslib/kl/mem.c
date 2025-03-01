/* Project:     OSLib
 * Description: The OS Construction Kit
 * Date:                1.6.2000
 * Idea by:             Luca Abeni & Gerardo Lamastra
 *
 * OSLib is an SO project aimed at developing a common, easy-to-use
 * low-level infrastructure for developing OS kernels and Embedded
 * Applications; it partially derives from the HARTIK project but it
 * currently is independently developed.
 *
 * OSLib is distributed under GPL License, and some of its code has
 * been derived from the Linux kernel source; also some important
 * ideas come from studying the DJGPP go32 extender.
 *
 * We acknowledge the Linux Community, Free Software Foundation,
 * D.J. Delorie and all the other developers who believe in the
 * freedom of software and ideas.
 *
 * For legalese, check out the included GPL license.
 */

/*	Basic Memory Manager Functions:
	A classic fixed partition allocator!	*/

#include <ll/i386/stdlib.h>
#include <ll/i386/string.h>
#include <ll/i386/mem.h>
#include <ll/i386/cons.h>
#include <ll/i386/error.h>

#include <ll/sys/ll/ll-mem.h>

FILE(Memory);

#define MAX_PARTITION   50 /* Max available partition */

static struct {
	BYTE used;
#ifdef __WC16__
	BYTE __huge *addr;
#else
	BYTE *addr;
#endif
	DWORD size;
} mem_table[MAX_PARTITION];

void ll_mem_init(void *base, DWORD size)
{
    register int i;
    mem_table[0].used = 1;
    mem_table[0].addr = base;
    mem_table[0].size = size;
    for (i = 1; i < MAX_PARTITION; i++) mem_table[i].used = 0;
}

void *ll_alloc(DWORD s)
{
    void *p = NULL;
    int i = 0;

    while (i < MAX_PARTITION && p == NULL) {
	if (mem_table[i].used && (mem_table[i].size >= s))
		p = mem_table[i].addr;
	else i++;
    }
    if (p != NULL) {
	if (mem_table[i].size > s) {
	    mem_table[i].size -= s;
	    mem_table[i].addr += s;
	}
	else mem_table[i].used = FALSE;
    }
    return(p);
}

WORD ll_free(void *p,DWORD s)
{
    register int i = 1;
    unsigned i1 = 0, i2 = 0;

    while (i < MAX_PARTITION && ((i1 == 0) || (i2 == 0)) ) {
	if (mem_table[i].used) {
	    if (mem_table[i].addr + mem_table[i].size == p) i1 = i;
	    if (mem_table[i].addr == (BYTE *)(p) + s) i2 = i;
	}
	i++;
    }
    if (i1 != 0 && i2 != 0) {
	mem_table[i1].size += mem_table[i2].size + s;
	mem_table[i2].used = FALSE;
    }
    else if (i1 == 0 && i2 != 0) {
	mem_table[i2].addr = p;
	mem_table[i2].size += s;
    }
    else if (i1 != 0 && i2 == 0) mem_table[i1].size += s;
    else {
	i = 0;
	while (i < MAX_PARTITION && (mem_table[i].used == TRUE)) i++;
	if (i == MAX_PARTITION) return(FALSE);
	mem_table[i].addr = p;
	mem_table[i].size = s;
	mem_table[i].used = TRUE;
    }
    return(TRUE);
}

void ll_mem_dump(void)
{
    register int i;
    for (i = 0; i < MAX_PARTITION; i++) {
	if (mem_table[i].used) message("Entry : [%d] Addr : %p Size : %ld\n",i,mem_table[i].addr,mem_table[i].size);
    }
}
