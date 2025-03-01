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

/*	Safe abort routine & timer asm handler	*/

.title	"VmCx32a.S"

#include <ll/i386/sel.h>
#include <ll/i386/linkage.h>
#include <ll/i386/defs.h>

#include <ll/sys/ll/exc.h>

.data          

ASMFILE(Context)

.extern JmpSel
.extern JmpZone
.globl SYMBOL_NAME(ll_clock)
.globl SYMBOL_NAME(SafeStack)
		
		/* Used as JMP entry point to check if a real	*/
		/* context switch is necessary			*/
		
SYMBOL_NAME_LABEL(ll_clock)	.int	0

		/* Safe stack area for aborts	*/		
.space		4096,0
SYMBOL_NAME_LABEL(SafeStack)

.extern  SYMBOL_NAME(periodic_wake_up)
.extern  SYMBOL_NAME(oneshot_wake_up)
.extern  SYMBOL_NAME(act_int)        
.extern  SYMBOL_NAME(abort_tail)

.text
		.globl SYMBOL_NAME(ll_timer)
		.globl SYMBOL_NAME(ll_abort)

SYMBOL_NAME_LABEL(ll_abort)
			/* As we are terminating we cannnot handle */
			/* any other interrupt!			   */
			cli
			/* Get argument */
			movl    4(%esp),%eax
			/* Switch to safe stack */
			movl    $(SYMBOL_NAME(SafeStack)),%esp
			/* Push argument	*/
			pushl	%eax
			/* Call sys_abort(code) */
			call    SYMBOL_NAME(abort_tail)

/* This is the timer handler; it reloads for safety the DS register; this   */
/* prevents from mess when timer interrupts linear access to memory (not in */
/* ELF address space); then EOI is sent in order to detect timer overrun    */
/* The high level kernel procedure wake_up() is called to perform the 	    */
/* preeption at higher level (process descriptos); the resulting context    */
/* if different from the old one is used to trigger the task activation.    */

SYMBOL_NAME_LABEL(ll_timer)
			pushal
			pushw	%ds
			pushw	%es
			pushw	%fs
			pushw	%gs
			
			movw    $(X_FLATDATA_SEL),%ax
			movw    %ax,%ds
			movw    %ax,%es

			/* Send EOI to master PIC		*/
			/* to perform later the overrun test	*/
			movb    $0x20,%al
			movl	$0x20,%edx
			outb    %al,%dx

			/* Call wake_up(actual_context) */
			
			cld
			movl	SYMBOL_NAME(ll_clock),%eax
			incl 	%eax
			movl	%eax,SYMBOL_NAME(ll_clock)
			
			xorl    %eax,%eax
#if 0
			strw    %ax
			pushl   %eax
			call    SYMBOL_NAME(wake_up)
			addl    $4,%esp
			pushl   %eax     /* Save result on stack */
			
#else
			movl	SYMBOL_NAME(timermode), %eax
			cmpl	$1, %eax
			je	oneshot	
			call    SYMBOL_NAME(periodic_wake_up)
			jmp	goon
oneshot:		call    SYMBOL_NAME(oneshot_wake_up)
goon:	
#endif
			/* This is the overrun test		 */
			/* Do it after sending EOI to master PIC */
					
			movb    $0x0A,%al
			movl	$0x020,%edx
			outb    %al,%dx
			inb     %dx,%al
			testb   $1,%al
			jz      Timer_OK
			movl    $(CLOCK_OVERRUN),%eax
			pushl	%eax
			call    SYMBOL_NAME(ll_abort)

#ifdef __VIRCSW__
Timer_OK:

			movw	SYMBOL_NAME(currCtx), %ax
			cmpw    %ax,JmpSel
			je      NoPreempt2
			movw    %ax,JmpSel
			ljmp    JmpZone  /* DISPATCH! */
#else
Timer_OK:
#endif
NoPreempt2:
			popw	%gs
			popw	%fs
			popw	%es
			popw	%ds
			popal
			iret
