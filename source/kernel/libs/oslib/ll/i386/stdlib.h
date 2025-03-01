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

/* Standard library for OSLib applications */

#ifndef __LL_I386_STDLIB_H__
#define __LL_I386_STDLIB_H__

#include <ll/i386/defs.h>
BEGIN_DEF

#define EXIT_FAILURE    1       /* Failing exit status.  */
#define EXIT_SUCCESS    0       /* Successful exit status.  */

#ifndef NULL
    #define NULL 0L
#endif

#define RAND_MAX        2147483647

typedef int wchar_t;

/* String conversion functions  	*/
/* File: StrConv.C			*/

long strtoi(char *s,int base,char **scan_end);
unsigned long strtou(char *s,int base,char **scan_end);
double strtod(char *s,char **scan_end);
long strtol(const char *nptr, char **endptr, int base);
unsigned long strtoul(const char *nptr, char **endptr, int base);


unsigned ecvt(double v,char *buffer,int width,int prec,int flag);
unsigned fcvt(double v,char *buffer,int width,int prec,int flag);
unsigned gcvt(double v,char *buffer,int width,int prec,int flag);
unsigned dcvt(long v,char *buffer,int base,int width,int flag);
unsigned ucvt(unsigned long v,char *buffer,int base,int width,int flag);


/* StdLib Macro */

#define atof(s)	strtod(s, NULL);
#define atoi(s)	strtoi(s, 10, NULL);
#define atou(s)	strtou(s, 10, NULL);
#define atol(s) strtol(s, 10, NULL);

/* Generic utility functions 	*/
/* File StdLib.C		*/

void srand(long int seed);
long int rand(void);
unsigned abs(int x);

/* The stdlib exit functions */
void exit(int code);


/* Stdlib Macro */
#ifndef __WC16__
#define labs(x)		abs(x)
#endif

#if !defined(__max)
#define __max(a,b)  (((a) > (b)) ? (a) : (b))
#endif
#if !defined(max) && !defined(__cplusplus)
#define max(a,b)  (((a) > (b)) ? (a) : (b))
#endif
#if !defined(__min)
#define __min(a,b)  (((a) < (b)) ? (a) : (b))
#endif
#if !defined(min) && !defined(__cplusplus)
#define min(a,b)  (((a) < (b)) ? (a) : (b))
#endif
END_DEF

#endif
