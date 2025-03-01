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

/*	Kernel Library data structures definitions	*/

#ifndef __LL_SYS_LL_LL_DATA_H__
#define __LL_SYS_LL_LL_DATA_H__

#include <ll/i386/defs.h>
BEGIN_DEF

#include <ll/i386/hw-data.h>
#include <ll/i386/hw-instr.h>
#include <ll/i386/hw-func.h>

/* These are used by the ll_init function... */
#define LL_PERIODIC 0
#define LL_ONESHOT 1

struct ll_initparms {
	DWORD mode;
	TIME tick;
};

END_DEF
#endif /* __LL_SYS_LL_LL_DATA_H__ */
