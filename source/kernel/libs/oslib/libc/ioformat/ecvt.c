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

unsigned ecvt(double v,char *buffer,int width,int prec,int flag)
{
    register int len=0;
    long int exponent,abs_exponent;
    /* Manage Inf & NaN */
    if (isspecial(v,buffer)) return(strlen(buffer));
    /* Check for negative value & add optional + sign */
    if (v < 0) {
	*buffer++ = '-';
	v = -v;
	len++;
    } else if (flag & ADD_PLUS) {
	*buffer++ = '+';
	len++;
    }	
    /* Zero check! */
    if (v < DBL_MIN) {
	*buffer++ = '0';
	*buffer = 0;
	return(1);
    }
    /* Evaluate the exponent */
    if (v < 1) {
	exponent = 0;
	while (v < 1.0) {
	    v *= 10.0;
	    exponent--;
	}
    }
    else {
	exponent = 0;
	while (v >= 10.0) {
	    v /= 10.0;
	    exponent++;
	}
    }
    abs_exponent = abs(exponent);
    if (abs_exponent > 99) width -= 3;
    else if (abs_exponent > 9) width -= 2;
    else if (exponent > 0) width -= 1;
    /* Now the number as IP in range [0,1] */
    /* Convert this as a fixed point format */
    len += fcvt(v,buffer,width,prec,flag | RESPECT_WIDTH);
    /* Now convert the exponent */
    if (exponent == 0) return(len);
    buffer += len;
    *buffer++ = 'e';
    len += dcvt(exponent,buffer,10,exponent/10+2,ADD_PLUS)+1;
    return(len);
}
