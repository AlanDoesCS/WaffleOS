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

/*	Address spaces code & data	*/

#ifndef __LL_SYS_LL_ASPACE_H__
#define __LL_SYS_LL_ASPACE_H__

#include <ll/i386/defs.h>
BEGIN_DEF

/* I dont't know if we really need all these things... */
#include <ll/i386/hw-data.h>
#include <ll/i386/hw-instr.h>
#include <ll/i386/hw-func.h>
#include <ll/i386/tss-ctx.h>

struct as {
	DWORD base;
	DWORD limit;
	WORD status;
};
/* An Address Space descriptor is a Segment descriptor... so it is a WORD... */
#define AS WORD

#define AS_FREE 0
#define AS_BUSY 1

#define ASMax 60

#if 0
#define ASBase                 0x300	/* Is it correct? TSSBase + 64 *8... */
#endif

#define ASBase                 (TSSBase + TSSMax * 8)
#define ASsel2index(sel)       ((sel-ASBase) / 16)
#define ASindex2sel(i)         (ASBase + i * 16)


void as_init(void);
AS as_create(void);
int as_bind(AS as, DWORD ph_addr, DWORD l_addr, DWORD size);


END_DEF

#endif /* __LL_SYS_LL_ASPACE_H__ */
