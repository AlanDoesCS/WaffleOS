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

/* This file contains the DOS memory allocator 	*/

#include <ll/i386/mem.h>
#include <ll/i386/x-bios.h>
#include <ll/i386/x-dosmem.h>
#include <ll/i386/error.h>

FILE(X-Dos-Memory);

/*
    We do not use the standard K&R pointer based memory allocator!
    This is because we cannot do pointer tricks easily with the GCC
    which only handles explicitely 32 bit non segmented address space.
    So we cannot use far pointers, and we cannot chain buckets of memory
    which are located in low memory area, beyond the accessibility
    of the ELF address space.
    A static allocation is so preferred; this should not give trouble
    since DOS memory is only used for reflection related purposes
*/

#define MAX_PARTITION   50 /* Max available partition */

static struct {
	BYTE 	 used;              
	LIN_ADDR addr;
	DWORD 	 size;                     
} mem_table[MAX_PARTITION];

void DOS_dump_mem(void)
{
    register int i;
    for (i = 0; i < MAX_PARTITION; i++) {
	if (mem_table[i].used) message("(%d) Addr : %p   Size : %lu/%lx\n",
			i, mem_table[i].addr,
			mem_table[i].size, mem_table[i].size);
    }
}

void DOS_mem_init(void)
{
    register int i;
    mem_table[0].used = TRUE;
    X_meminfo(NULL,NULL,&(mem_table[0].addr),&(mem_table[0].size));
    for (i = 1; i < MAX_PARTITION; i++)	mem_table[i].used = FALSE;
}

LIN_ADDR DOS_alloc(DWORD s)
{
    LIN_ADDR p = 0;
    int i = 0;

    while (i < MAX_PARTITION && p == NULL) {
	if (mem_table[i].used && (mem_table[i].size >= s))
		p = mem_table[i].addr;
	else i++;
    }
    if (p != 0) {
	if (mem_table[i].size > s) {
	    mem_table[i].size -= s;
	    mem_table[i].addr += s;
	}
	else mem_table[i].used = FALSE;
    }
    return(p);
}

int DOS_free(LIN_ADDR p,DWORD s)
{
    register int i = 1;
    unsigned i1 = 0, i2 = 0;

    while (i < MAX_PARTITION && ((i1 == 0) || (i2 == 0)) ) {
	if (mem_table[i].used) {
	    if (mem_table[i].addr + mem_table[i].size == p) i1 = i;
	    if (mem_table[i].addr == p + s) i2 = i;
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
