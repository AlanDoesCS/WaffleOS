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

/*	Context switch code	*/

#include <ll/i386/hw-data.h>
#include <ll/i386/hw-func.h>
#include <ll/i386/tss-ctx.h>

FILE(Context-Switch);

extern unsigned short int currCtx;
#ifdef __VIRCSW__
extern int activeInt;
#endif

extern void context_load(CONTEXT c);

void ll_context_to(CONTEXT c)
{
#ifdef __VIRCSW__
	currCtx = c;
	if (activeInt == 0) {
		context_load(c);
	}
#else
	currCtx = c;
	context_load(c);
#endif
}
CONTEXT ll_context_from(void)
{
#ifdef __VIRCSW__
	return currCtx;
#else
	return context_save();
#endif
}

CONTEXT ll_context_save(void)
{
	return currCtx;
}

void ll_context_load(CONTEXT c)
{
	currCtx = c;
	context_load(c);
}
