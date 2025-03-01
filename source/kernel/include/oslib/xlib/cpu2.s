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

/*	CPU detection code	*/

.title	"CPU2.S"

#include <ll/i386/linkage.h>
#include <ll/i386/defs.h>

/*
 * The following code has been extracted by Intel AP-485
 * Application Note: Intel Processor Identification with CPUID instruction
 */

.data

ASMFILE(CPU2)

.globl	SYMBOL_NAME(X86_fpu)

SYMBOL_NAME_LABEL(X86_fpu)		.byte	0
SYMBOL_NAME_LABEL(X86_cpu)		.byte	0
fpu_status:	.word	0

/*
	struct CPU {
		DWORD X86_cpu;			0
		DWORD X86_cpuIdFlag;		4
		DWORD X86_vendor_1;		8
		DWORD X86_vendor_2;		12
		DWORD X86_vendor_3;		16
		DWORD X86_signature;		20
		DWORD X86_IntelFeature_1;	24
		DWORD X86_IntelFeature_2;	28
		DWORD X86_StandardFeature;	32
	}
*/

.text

.globl SYMBOL_NAME(X86_get_CPU)
.globl SYMBOL_NAME(X86_get_FPU)
.globl SYMBOL_NAME(X86_is386)
.globl SYMBOL_NAME(X86_isCyrix)
.globl SYMBOL_NAME(X86_hasCPUID)

SYMBOL_NAME_LABEL(X86_is386)
		pushfl
		popl	%eax
		movl	%eax, %ecx
		xorl	$0x40000, %eax
		pushl	%eax
		popfl
		pushfl
		popl	%eax
		cmp	%ecx, %eax
		jz	is386
		
		pushl	%ecx
		popfl	
		movl	$0, %eax
		ret
is386:
		movl	$1, %eax
		ret

SYMBOL_NAME_LABEL(X86_hasCPUID)
		pushfl
		popl	%eax
		movl	%eax, %ecx
		xorl	$0x200000, %eax
		pushl	%eax
		popfl
		pushfl
		popl	%eax
		xorl	%ecx, %eax
		je	noCPUID

		pushl	%ecx	/* Restore the old EFLAG value... */
		popfl
		movl	$1, %eax
		ret
noCPUID:
		movl	$0, %eax
		ret

SYMBOL_NAME_LABEL(X86_isCyrix)
		xorw	%ax, %ax
		sahf
		mov	$5, %ax
		mov	$2, %bx
		divb	%bl
		lahf
		cmpb	$2, %ah
		jne	noCyrix
		movl	$1, %eax
		ret
noCyrix:
		movl	$0, %eax
		ret

SYMBOL_NAME_LABEL(X86_get_FPU)	
		fninit
		movw	$0x5a5a, fpu_status
		fnstsw	fpu_status
		movw	fpu_status, %ax
		cmpb	$0, %al
		movb	$0, SYMBOL_NAME(X86_fpu)
		jne	endFPUProc

chkCW:		fnstcw 	fpu_status
		movw	fpu_status, %ax
		andw	$0x103f, %ax
		cmpw	$0x03F, %ax
		movb	$0, SYMBOL_NAME(X86_fpu)
		jne	endFPUProc
		movb	$1, SYMBOL_NAME(X86_fpu)
		
chkInf:		cmpb	$3, SYMBOL_NAME(X86_cpu)
		jle	endFPUProc
		fld1
		fldz
		fdivp
		fld	%st
		fchs
		fcompp
		fstsw	fpu_status
		movb	$2, SYMBOL_NAME(X86_fpu)
		sahf
		jz	endFPUProc
		movb	$3, SYMBOL_NAME(X86_fpu)
endFPUProc:	ret
