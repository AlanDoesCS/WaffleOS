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

unsigned gcvt(double v,char *buffer,int width,int prec,int flag)
{
    double v_abs;
    int exponent;
    /* Manage Inf & NaN */
    if (isspecial(v,buffer)) return(strlen(buffer));
    /* Extract absolute value */
    if (v < 0.0) v_abs = -v;
    else v_abs = v;
    /* Zero check! */
    if (v_abs < DBL_MIN) {
	*buffer++ = '0';
	*buffer = 0;
	return(1);
    }
    exponent = 0;
    /* Evaluate exponent */
    if (v_abs < 1.0) {
	while (v_abs < 1) {
	    v_abs *= 10.0;
	    exponent--;
	}
    }
    else {
	while (v_abs >= 10.0) {
	    v_abs /= 10.0;
	    exponent++;
	}
    }
    /* Choose shortest format on exponent value */
    if (exponent > 7 || exponent < -7) 
       return(ecvt(v,buffer,width,prec,flag));
    else return(fcvt(v,buffer,width,prec,flag));
}
