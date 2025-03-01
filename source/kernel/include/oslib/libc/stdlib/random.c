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
#include <ll/math.h>

#define MODULUS		2147483647L
#define FACTOR          16807L
#define DIVISOR  	127773L
#define REMAINDER       2836L
static long int last_val;

long int rand(void)
{
 long int last_div = last_val / DIVISOR;
 long int last_rem = last_val % DIVISOR;
 last_val = (FACTOR * last_rem) - (REMAINDER * last_div);
 if (last_val < 0) last_val += MODULUS;
 return(last_val);
}

void srand(long int seed)
{
    last_val = seed;
}

