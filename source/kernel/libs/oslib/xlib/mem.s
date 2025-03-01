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

/*	Memcopy to other address spaces	*/

#include <ll/i386/linkage.h>
#include <ll/i386/defs.h>

.data          

ASMFILE(Context)

.text

	.globl SYMBOL_NAME(fmemcpy)

/* void fmemcpy(unsigned short ds,unsigned long do,
		unsigned short ss,unsigned long so,unsigned n) */
		
SYMBOL_NAME_LABEL(fmemcpy)
	/* Build the standard stack frame */
		pushl	%ebp
		movl	%esp,%ebp
		pushl	%esi
		pushl	%edi
		pushw	%ds
		pushw	%es
		
		/* Get parms into register */
		movl	8(%ebp),%eax
		movw	%ax,%es
		movl	12(%ebp),%edi
		movl	16(%ebp),%eax
		movw	%ax,%ds
		movl	20(%ebp),%esi		
		movl	24(%ebp),%ecx
		cld
		rep 
		movsb
		popw	%es
		popw	%ds	
		popl	%edi
		popl	%esi		
		leave
		ret
