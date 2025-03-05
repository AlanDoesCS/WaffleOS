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

/*	Hardware exceptions	*/

#ifndef __LL_SYS_LL_HW_EXC_H__
#define __LL_SYS_LL_HW_EXC_H__

#define DIV_BY_0		0 /* These are the ll... exceptions */
#define MATH_EXC		1
#define NMI_EXC			2
#define DEBUG_EXC		3
#define BREAKPOINT_EXC		4
#define HW_FAULT		5
#define NO_MORE_HW_DESC		6
#define VM86_PANIC		7

#define CLOCK_OVERRUN		64 /* Warning this is used in vm1.asm   */

#endif /* __LL_SYS_LL_HW_EXC_H__ */

