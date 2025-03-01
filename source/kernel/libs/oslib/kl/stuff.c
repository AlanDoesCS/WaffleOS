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

/*	ll_* 32 bit utility functions: here, there are all the things that
	we don't know where to put	*/


/* This is declared in the LLCtx.Asm/S file */
/* Timer 0 counter */

#include <ll/i386/stdlib.h>
#include <ll/i386/string.h>
#include <ll/i386/stdio.h>
#include <ll/i386/mem.h>
#include <ll/i386/cons.h>

#include <ll/stdarg.h>

FILE(Stuff);

int ll_printf(char *fmt,...)
{
    char buf[100];
    va_list parms;
    int result;

    va_start(parms,fmt);
    result = vksprintf(buf,fmt,parms);
    cputs(buf);
    va_end(parms);
    return(result);
}
