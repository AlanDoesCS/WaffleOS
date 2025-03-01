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

/*	The hw I/O instructions
	Never include this file!!! Include hw-instr.h instead!!!	*/
#ifndef __LL_I386_HW_IO_H__
#define __LL_I386_HW_IO_H__

#include <ll/i386/defs.h>
BEGIN_DEF


/*
    The code for inp, outp, inpw, outpw, inpd & outpd is from
    standard GNU C distribution!
    The full source code for the GNU-C distribution is available
    from www.delorie.com

    The following code is ... 
    copyright (C) 1995 DJ Delorie, see COPYING.DJ for details
*/

INLINE_OP unsigned char inp(unsigned short _port)
{
    unsigned char rv;
    __asm__ __volatile__ ("inb %1, %0"
	  : "=a" (rv)
	  : "d" (_port));
    return(rv);
}

INLINE_OP unsigned short inpw (unsigned short _port)
{
    unsigned short rv;
    __asm__ __volatile__ ("inw %1, %0"
	  : "=a" (rv)
	  : "d" (_port));
    return(rv);
}

INLINE_OP unsigned long inpd(unsigned short _port)
{
    unsigned long rv;
    __asm__ __volatile__ ("inl %1, %0"
	  : "=a" (rv)
	  : "d" (_port));
    return(rv);
}

INLINE_OP void outp(unsigned short _port, unsigned char _data)
{
    __asm__ __volatile__ ("outb %1, %0"
	  :
	  : "d" (_port),
	    "a" (_data));
}

INLINE_OP void outpw(unsigned short _port, unsigned short _data)
{
    __asm__ __volatile__ ("outw %1, %0"
	  :
	  : "d" (_port),
	    "a" (_data));
}

INLINE_OP void outpd(unsigned short _port, unsigned long _data)
{
    __asm__ __volatile__ ("outl %1, %0"
	  :
	  : "d" (_port),
	    "a" (_data));
}

END_DEF

#endif
