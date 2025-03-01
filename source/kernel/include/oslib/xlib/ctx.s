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

#include <ll/i386/sel.h>
#include <ll/i386/linkage.h>
#include <ll/i386/defs.h>

#include <ll/sys/ll/exc.h>

.data          

ASMFILE(Context)

.globl SYMBOL_NAME(currCtx)
.globl JmpSel
.globl JmpZone

SYMBOL_NAME_LABEL(currCtx)	.word   0

JmpZone:
JmpOffset:	
.word	0
.word	0
JmpSel:	
.word	0		 

.text

		.globl SYMBOL_NAME(context_save)
		.globl SYMBOL_NAME(context_change)
		.globl SYMBOL_NAME(context_load)
		.globl SYMBOL_NAME(init_TR)

/* This function assign a value to the TASK register of 386 architecture */
/* We MUST do this BEFORE we use the HW multitask			 */

SYMBOL_NAME_LABEL(init_TR)
		pushl	%ebp
		movl	%esp,%ebp
		movl	8(%ebp),%eax
		ltrw	%ax
		movw	%ax,JmpSel
		movw	%ax,SYMBOL_NAME(currCtx)
		popl	%ebp
		ret
		
/* CONTEXT __cdecl context_save(void);				  */
/* The context is returned into AX; Interrupts are also cleared as we are */
/* entering into kernel primitives					  */

SYMBOL_NAME_LABEL(context_save)
			xorl     %eax,%eax
			strw     %ax
			ret

/* void __cdecl context_change(CONTEXT c)				*/
/* Use 386 task switch ability. This is the last call of any preemption */
/* generating primitive; when the original task is re-activated the 	*/
/* interrupt flag is restored with STI					*/
/* In 32 bit systems, context_load is an alias for context_change!*/

SYMBOL_NAME_LABEL(context_load)
SYMBOL_NAME_LABEL(context_change)	
			pushl   %ebp
			movl    %esp,%ebp
			movw	$(X_FLATDATA_SEL),%ax
			movw	%ax,%ds
			movw	%ax,%es
			movl    8(%ebp),%eax
			cmpw    JmpSel,%ax
			je      NoPreempt
			movw    %ax,JmpSel
			ljmp    JmpZone
NoPreempt:              popl	%ebp
			ret

