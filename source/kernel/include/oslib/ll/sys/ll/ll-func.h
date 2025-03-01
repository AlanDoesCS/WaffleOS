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

/*	Kernel Library functions interfaces	*/

#ifndef __LL_SYS_LL_LL_FUNC_H_
#define __LL_SYS_LL_LL_FUNC_H_

#include <ll/i386/defs.h>
BEGIN_DEF

#include <ll/i386/hw-data.h>
#include <ll/i386/hw-instr.h>
#include <ll/i386/hw-func.h>

#include <ll/sys/ll/ll-data.h>

void ll_context_setspace(CONTEXT c, WORD as);
CONTEXT ll_context_create(void (*task)(void *p),BYTE *stack,
				void *parm,void (*killer)(void),WORD ctrl);

/* Release a used task context						  */
void ll_context_delete(CONTEXT c);

/* Put the context value into human readable form; used for debug! */
char *ll_context_sprintf(char *str,CONTEXT c);

/* These functions start-up & close the ll layer */

void *ll_init(void);
void ll_end(void);

/* This functions acts as safety place where to go when any error */
/* occurs and we do not know what context is active		  */
void ll_abort(int code);

BEGIN_DEF
#endif /* __LL_SYS_LL_LL_MEM_H_ */
