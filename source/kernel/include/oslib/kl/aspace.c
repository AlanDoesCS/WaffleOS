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

/*	Routines to create address spaces and bind physical memory to them */

#include <ll/sys/ll/ll-data.h>
#include <ll/sys/ll/aspace.h>

FILE(Address-Space);

struct as AS_table[ASMax];

void as_init(void)
{
	int i;

	for (i = 0; i < ASMax; i++) {
		AS_table[i].status = AS_FREE;
	}
}

AS as_create(void)
{
	int i;

	i = 0;
	while((i < ASMax) && (AS_table[i].status & AS_BUSY)) {
		i++;
	}
	
	if (i == ASMax) {
		return 0;
	}
	
	AS_table[i].status = AS_BUSY;	/* Empty address space... */
	AS_table[i].base = 0;
	AS_table[i].limit= 0;

	GDT_place(ASindex2sel(i), AS_table[i].base, AS_table[i].limit, 
			DATA_ACCESS, GRAN_32B);
	GDT_place(ASindex2sel(i) + 8, AS_table[i].base, AS_table[i].limit, 
			CODE_ACCESS, GRAN_32B);
/* We also need a code segment... */

	return ASindex2sel(i);
}

int as_bind(AS as, DWORD ph_addr, DWORD l_addr, DWORD size)
{
	int i = ASsel2index(as);

	/* We have not paging... So l_addr must be 0 */
	if (l_addr != 0)
		return -1;

	AS_table[i].base = ph_addr;
	AS_table[i].limit= size;

	GDT_place(as, AS_table[i].base, AS_table[i].limit, 
			DATA_ACCESS, GRAN_32B);
	GDT_place(as + 8, AS_table[i].base, AS_table[i].limit, 
			CODE_ACCESS, GRAN_32B);
/* We also need a code segment... */

	return 1;
}
