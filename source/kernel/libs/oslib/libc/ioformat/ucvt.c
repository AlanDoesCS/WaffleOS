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

#include <ll/i386/stdlib.h>
#include <ll/i386/string.h>
#include <ll/i386/limits.h>
#include <ll/i386/float.h>
#include <ll/i386/mem.h>
#include <ll/stdarg.h>
#include <ll/ctype.h>
#include <ll/math.h>
#include "sprintf.h"

unsigned ucvt(unsigned long v,char *buffer,int base,int width,int flag)
{
    register int i = 0,j;
    unsigned ret = 0,abs_base;
    unsigned long abs_v;
    char tmp[12];
    /* Extract the less significant digit */
    /* Put it into temporary buffer       */
    /* It has to be local to have 	  */
    /* reentrant functions		  */
    /*
    MG: fix to handle zero correctly
    if (v == 0) {
	*buffer++ = '0';
	*buffer = 0;
	return(1);
    }
    */
    
    /* MG: is this required? */
    /* the vsprintf() function seems to call this procedure with */
    /* this flag inverted */
    flag ^= LEFT_PAD;
    
    abs_base = (base >= 0) ? base : -base;
    if (base < 0) abs_v = ((long)(v) < 0) ? -v : v;
    else abs_v = v;
    /* Process left space-padding flags */
    if (flag & ADD_PLUS || ((base < 0) && ((long)(v) < 0))) {
	ret++;
    }
    /* MG: fix to handle zero correctly */
    if (v == 0)
        tmp[i++]='0';
    else
        while (abs_v > 0) {
	    tmp[i++] = todigit(abs_v % abs_base);
	    abs_v = abs_v / abs_base;
        }
    ret += i;
    if ((flag & LEFT_PAD) && (flag & SPACE_PAD)) {
	j = ret;
	while (j < width) {
	    *buffer++ = ' ';
	    ret++;
	    j++;
	}
    }
    /* Add plus if wanted */
    if (base < 0) {
	if (((long)(v)) < 0) *buffer++ = '-';
        else if (flag & ADD_PLUS) *buffer++ = '+';
    } else if (flag & ADD_PLUS) *buffer++ = '+';
    /* Process left zero-padding flags */
    if ((flag & LEFT_PAD) && (flag & ZERO_PAD)) {
	j = ret;
	while (j < width) {
	    *buffer++ = '0';
	    ret++;
	    j++;
	}
    }
    /* Reverse temporary buffer into output buffer */
    /* If wanted left pad with zero/space; anyway only one at once is ok */
    for (j = i-1; j >= 0; j--) *buffer++ = tmp[j];
    if ((flag & (SPACE_PAD)) && !(flag & LEFT_PAD)) {
	/* If wanted, pad with space to specified width */
	j = ret;
	while (j < width) {
	    *buffer++ = ' ';
	    ret++;
	    j++;
	}
    }
    /* Null terminate the output buffer */
    *buffer = 0;
    return(ret);
}

unsigned dcvt(long v,char *buffer,int base,int width,int flag)
{
    return(ucvt((unsigned long)(v),buffer,-base,width,flag));
}
