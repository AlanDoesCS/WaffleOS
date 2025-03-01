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
#include <ll/stdarg.h>
#include <ll/ctype.h>
#include <ll/math.h>

double strtod(char *s,char **scan_end)
{
    int sign,i;
    double result = 0;
    double value;
    double mantissa = 0,divisor = 1;
    unsigned short power = 0;
    /* Evaluate sign */
    if (*s == '-') {
	sign = -1;
	s++;
    }
    else sign = 1;
    /* Skip trailing zeros */
    while (*s == '0') s++;
    /* Convert integer part */
    while (*s <= '9' && *s >= '0') {
	value = *s++ - '0';
	result *= 10.0;
	result += value;
    }
    /* Detect floating point & mantissa */
    if (*s == '.') {
	s++;
    	while (*s <= '9' && *s >= '0') {
	    value = *s++ - '0';
	    mantissa *= 10.0;
	    mantissa += value;
	    divisor *= 10.0;
	}
    }
    mantissa /= divisor;
    /* Adjust result */
    result += mantissa;
    /* Adjust sign */
    result *= sign;
    /* Detect exponent */
    if (*s == 'e' || *s == 'E') {
	s++;
	if (*s == '-') {
	    sign = -1;
	    s++;
	} else if (*s == '+') {
	    sign = 1;
	    s++;
	}
	else sign = 1;
    	while (*s <= '9' && *s >= '0') {
	    value = *s++ - '0';
	    power *= 10.0;
	    power += value;
	}
    }
    /* Adjust result on exponent sign */
    if (sign > 0) for (i = 0; i < power; i++) result *= 10.0;
    else for (i = 0; i < power; i++) result /= 10.0;
    if (scan_end != 0L) *scan_end = s;
    return(result);
}
