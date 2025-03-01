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

/*	Inline functions for managing timespec structures.
	All timespec values are pointers!!!
	This file defines these functions:
		TIMESPEC2NANOSEC(t)
		converts a timespec value to a nanosec value, and return
			it, no checks
		TIMESPEC2USEC(t)
		converts a timespec value to a nanosec value, and return
		it, no checks
		NULL_TIMESPEC(t)
		the timespec value is set to the Epoch (=0)
		ADDNANO2TIMESPEC(n, t)
		t = t + n
		ADDUSEC2TIMESPEC(m, t)
		t = t + m
		SUBTIMESPEC(s1, s2, d)
		d = s1 - s2
		ADDTIMESPEC(s1, s2, d)
		d = s1 + s2
		TIMESPEC_A_LT_B(a,b)
		a < b
		TIMESPEC_A_GT_B(a,b)
		a > b
		TIMESPEC_A_EQ_B(a,b)
		a == b
		TIMESPEC_A_NEQ_B(a,b)
		a != b
		TIMESPEC_ASSIGN(t1,t2)
		t1 = t2	*/

#ifndef __LL_SYS_LL_TIME_H__
#define __LL_SYS_LL_TIME_H__

#include <ll/i386/defs.h>
BEGIN_DEF

struct timespec {
	long	tv_sec;         /* Seconds */
	long    tv_nsec;        /* Nanoseconds */
};

/*
 * these macros come from the Utah Flux oskit...
 */

#define TIMESPEC2NANOSEC(t)     ((t)->tv_sec * 1000000000 + (t)->tv_nsec)
#define TIMESPEC2USEC(t)     ((t)->tv_sec * 1000000 + (t)->tv_nsec / 1000)
#define NULL_TIMESPEC(t)        ((t)->tv_sec = (t)->tv_nsec = 0)
#define ADDNANO2TIMESPEC(n, t)  ((t)->tv_nsec += (n), \
		(t)->tv_sec += (t)->tv_nsec / 1000000000, \
		(t)->tv_nsec %= 1000000000)

#define SUBTIMESPEC(s1, s2, d)                               \
	((d)->tv_nsec = ((s1)->tv_nsec >= (s2)->tv_nsec) ?   \
	 (((d)->tv_sec = (s1)->tv_sec - (s2)->tv_sec),       \
	  (s1)->tv_nsec - (s2)->tv_nsec)                     \
	 :                                                   \
	 (((d)->tv_sec = (s1)->tv_sec - (s2)->tv_sec - 1),   \
	  (1000000000 + (s1)->tv_nsec - (s2)->tv_nsec)))

/*
 * ...and these not!
 */


#define ADDTIMESPEC(s1, s2, d)                                           \
        (((d)->tv_nsec = ((s1)->tv_nsec + (s2)->tv_nsec)) % 1000000000), \
         ((d)->tv_sec = (s1)->tv_sec + (s2)->tv_sec) +                   \
                        ((s1)->tv_nsec + (s2)->tv_nsec)) / 1000000000) )

#define ADDUSEC2TIMESPEC(m, t)  ((t)->tv_nsec += (m%1000000)*1000,        \
		(t)->tv_sec += ((t)->tv_nsec / 1000000000) + (m/1000000), \
		(t)->tv_nsec %= 1000000000)

#define TIMESPEC_A_LT_B(a,b)                                            \
        (                                                               \
        ((a)->tv_sec <  (b)->tv_sec) ||                                 \
        ((a)->tv_sec == (b)->tv_sec && (a)->tv_nsec < (b)->tv_nsec)    \
        )

#define TIMESPEC_A_GT_B(a,b)                                            \
        (                                                               \
        ((a)->tv_sec >  (b)->tv_sec) ||                                 \
        ((a)->tv_sec == (b)->tv_sec && (a)->tv_nsec > (b)->tv_nsec)    \
        )

#define TIMESPEC_A_EQ_B(a,b)                                            \
        ((a)->tv_sec == (b)->tv_sec && (a)->tv_nsec == (b)->tv_nsec)

#define TIMESPEC_A_NEQ_B(a,b)                                            \
        ((a)->tv_sec != (b)->tv_sec || (a)->tv_nsec != (b)->tv_nsec)

#define TIMESPEC_ASSIGN(t1,t2) \
        ((t1)->tv_sec = (t2)->tv_sec, (t1)->tv_nsec = (t2)->tv_nsec)

#if 0
#define PITSPEC2TIMESPEC(a,b) \
     ((b)->tv_nsec = (((DWORD)((a)->units) * 1000) / 1197) * 1000, \
	(b)->tv_sec = ((a)->gigas * 1197) / 1000) /*, \
	(b)->tv_sec += (b)->tv_nsec / 1000000000, \
	(b)->tv_nsec %= 1000000000)     */
#else
/*#define PITSPEC2TIMESPEC(a,b) \
     ((b)->tv_nsec = (((DWORD)((a)->units) * 1000) / 1197) * 1000, \
        (b)->tv_nsec += (((a)->gigas * 1197) % 1000) * 1000000, \
	(b)->tv_sec = ((a)->gigas * 1197) / 1000 , \
	(b)->tv_sec += (b)->tv_nsec / 1000000000, \
	(b)->tv_nsec %= 1000000000)*/
#define PITSPEC2TIMESPEC(a,b) \
     ((b)->tv_nsec = (((DWORD)((a)->units) * 1000) / 1197), \
        (b)->tv_nsec += (((a)->gigas * 1197) % 1000) * 1000, \
	(b)->tv_sec = ((a)->gigas * 1197) / 1000 , \
	(b)->tv_sec += (b)->tv_nsec / 1000000, \
	(b)->tv_nsec %= 1000000, \
	(b)->tv_nsec *= 1000)
#endif

TIME ll_gettime(int mode, struct timespec *tsres);

#define TIME_PTICK	1
#define TIME_EXACT	2
#define TIME_NEW	3

END_DEF
#endif
