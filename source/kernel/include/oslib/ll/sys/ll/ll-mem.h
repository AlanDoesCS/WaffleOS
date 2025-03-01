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

/*	Some memory management code	*/

#ifndef __LL_SYS_LL_LL_MEM_H_
#define __LL_SYS_LL_LL_MEM_H_

#include <ll/i386/defs.h>
BEGIN_DEF

/* These function are used to manage memory at ll layer */

void * ll_alloc(DWORD size);
WORD ll_free(void *ptr,DWORD size);
void ll_mem_init(void *base,DWORD size);
void ll_mem_dump(void);

END_DEF
#endif /* __LL_SYS_LL_LL_MEM_H_ */
