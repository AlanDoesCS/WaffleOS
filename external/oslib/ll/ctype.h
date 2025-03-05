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

/*	Standard character conversions and tests	*/

#ifndef __LL_CTYPE_H__
#define __LL_CTYPE_H__

#include <ll/i386/defs.h>
BEGIN_DEF

/* String conversion functions */
char toupper(char c);
char tolower(char c);
int tonumber(char c);
char todigit(int c);
int isalnum(char c);
int isalpha(char c);
int iscntrl(char c);
int isdigit(char c);
int islower(char c);
int isspace(char c);
int isupper(char c);
int isxdigit(char c);
int isnumber(char c,int base);
int isspecial(double d,char *bufp);

END_DEF
#endif
