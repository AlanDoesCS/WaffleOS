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

/*	The POSIX float.h header	*/

#ifndef __LL_I386_FLOAT_H
#define __LL_I386_FLOAT_H__

#define DBL_DIG         15
#define DBL_EPSILON	2.22044604925031300e-016
#define DBL_MANT_DIG    53
#define DBL_MAX 	1.79769313486231500e+308
#define DBL_MAX_10_EXP  308
#define DBL_MAX_EXP     1024
#define DBL_MIN 	2.22507385850720200e-308
#define DBL_MIN_10_EXP  (-307)
#define DBL_MIN_EXP     (-1021)

#define FLT_DIG         6
#define FLT_EPSILON	1.192092896e-7f
#define FLT_MANT_DIG    24
#define FLT_MAX 	3.402823466e+38f
#define FLT_MAX_10_EXP  38
#define FLT_MAX_EXP     128
#define FLT_MIN 	1.175494351e-38f
#define FLT_MIN_10_EXP  (-37)
#define FLT_MIN_EXP     (-125)
#define FLT_RADIX       2
#define FLT_ROUNDS      1

#define LDBL_DIG        DBL_DIG
#define LDBL_EPSILON    DBL_EPSILON
#define LDBL_MANT_DIG   DBL_MANT_DIG
#define LDBL_MAX        DBL_MAX
#define LDBL_MAX_10_EXP DBL_MAX_10_EXP
#define LDBL_MAX_EXP    DBL_MAX_EXP
#define LDBL_MIN        DBL_MIN
#define LDBL_MIN_10_EXP DBL_MIN_10_EXP
#define LDBL_MIN_EXP    DBL_MIN_EXP

#endif
