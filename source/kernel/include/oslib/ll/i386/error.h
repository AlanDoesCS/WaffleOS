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

/* The LL error dump function */

#ifndef __LL_I386_ERROR_H__
#define __LL_I386_ERROR_H__

#include <ll/i386/defs.h>
BEGIN_DEF

#include <ll/i386/cons.h>

int  message(char *fmt,...) __attribute__((format(printf,1,2)));
#define error(msg) \
		message("Error! File:%s Line:%d %s", __FILE__, __LINE__, msg)
END_DEF

#endif

