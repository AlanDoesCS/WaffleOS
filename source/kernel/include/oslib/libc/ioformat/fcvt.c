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

unsigned fcvt(double v,char *buffer,int width,int prec,int flag)
{
    double ip,fp,val;
    register int l=0;
    register int digits;
    int conv,got_a_digit,exceeded,len = 0;
    char tmp[300];
    memset(tmp,0,300);
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
    /* Extract integer part & mantissa */
    fp = modf(v,&ip);
    /* Process integer part */
    digits = 0;
    if (ip >= 1.0) {
	while (ip >= 1.0) {
	    /* The 0.01 here is used to correct rounding errors*/
	    /* Ex: 1.2 --> 1.1999999999999... adjust to 1.2 */
	    /*
	    ip = ip / 10.0;
	    modf(ip,&val);
	    val = (ip - val)*10.0;
	    */
	    val = fmod(ip,10.0);
	    ip = ip / 10.0;
	    tmp[digits++] = todigit((unsigned)(val));
	}
	len += digits;
	/* Now reverse the temporary buffer into output buffer 	*/
	/* Translate only the last 15 digits 			*/
	/* The others are beyond double precision limit!  	*/	
	for (l = digits-1; l >= max(digits-15,0); l--) *buffer++ = tmp[l];
	if (l >= 0) for (l = digits-16; l >= 0; l--) *buffer++ = '0';
	*buffer = 0;
    /* If IP == 0 -> just put in a 0 */
    } else {
	*buffer++ = '0';
	len++;
    }
    /* Process fractionary part according to width specification */
    /* If RESPECT_WIDTH is set, scan until you reach wanted precision */
    /* Else scan until you find a not 0 digit */
    if (fp > 1e-307 && len < width+1) {
	*buffer++ = '.';
	len++;
	if (flag & RESPECT_WIDTH) got_a_digit = 1;
	else got_a_digit = 0;
	exceeded = 0;
	digits = 1;
	while (!exceeded) {
	    /* The DBL_EPSILON here is used to correct rounding errors */
	    fp = (fp + DBL_EPSILON) * 10.0;
	    fp = modf(fp,&ip);
	    conv = (int)(ip);
	    if (conv != 0 && !got_a_digit) got_a_digit = 1;
	    *buffer++ = todigit(conv);
	    len++;
	    digits++;
	    if (got_a_digit && (digits > prec)) exceeded = 1;
	    if (width < len) exceeded = 1;
	}
	/* Drop trailing zeros after decimal point */
        while (*--buffer == '0' && *(buffer-1) != '.') len--;
	buffer++;
    }
    *buffer = 0;
    return(len);
}


