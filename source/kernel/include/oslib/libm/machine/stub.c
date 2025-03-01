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


#include <ll/i386/cons.h>
#include <ll/stdlib.h>
#include <ll/sys/types.h>

/* bytes for +Infinity on a 387 */
char __infinity[] = { 0, 0, 0, 0, 0, 0, 0xf0, 0x7f };

static int (*__errnumber)(void) = NULL;
static int errno;
int *__errnumber1()
{
  if (__errnumber == NULL) {
    return &errno;
  }

  return (void *)__errnumber();
}

void seterrnumber(int (*e)(void))
{
  __errnumber = e;
}

int isnan(double d)
{
	register struct IEEEdp {
		u_int manl : 32;
		u_int manh : 20;
		u_int  exp : 11;
		u_int sign :  1;
	} *p = (struct IEEEdp *)&d;
	
	return(p->exp == 2047 && (p->manh || p->manl));
}

/* By Luca.... The write stub */
int write(int a, char *b, int c)
{
	b[c] = 0;
	cputs(b);
	return c;
}
