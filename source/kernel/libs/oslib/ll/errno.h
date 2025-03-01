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

/* Stuff needed by the Math library... It wants to set errno!!! */

#ifndef __LL_ERRNO_H__
#define __LL_ERRNO_H__

void seterrnumber(int *(*e)(void));
extern int *__errnumber1();

/*+ this macro refers the correct errno... +*/
#define errno (*__errnumber1())


#define EDOM	 33
#define EILSEQ   84
#define ERANGE	 34
#endif
