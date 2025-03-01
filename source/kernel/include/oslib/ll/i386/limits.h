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

/*	Some POSIX compliance stuff	*/

#ifndef __LL_I386_LIMITS_H__
#define __LL_I386_LIMITS_H__

/* Number limits */

#define CHAR_BIT	8
#define CHAR_MAX	255
#define CHAR_MIN	0
#define SCHAR_MAX	127
#define SCHAR_MIN	-128

#define INT_MAX    	2147483647
#define INT_MIN		(-INT_MAX - 1)
#define UINT_MAX  	4294967295U

#define LONG_MAX	2147483647L
#define LONG_MIN	(-LONG_MAX - 1)
#define ULONG_MAX	4294967295UL

#endif
