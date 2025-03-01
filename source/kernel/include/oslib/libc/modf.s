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

/*	Assembler portion of the library:
	function: modf!	*/

#include <ll/i386/linkage.h>

.text
.globl	SYMBOL_NAME(modf)

SYMBOL_NAME_LABEL(modf)
	pushl	%ebp
	movl	%esp,%ebp
	subl	$16,%esp
	pushl	%ebx
	fnstcw	-4(%ebp)
	fwait
	movw	-4(%ebp),%ax
	orw	$0x0c3f,%ax
	movw	%ax,-8(%ebp)
	fldcw	-8(%ebp)
	fwait
	fldl	8(%ebp)
	frndint
	fstpl	-16(%ebp)
	fwait
	movl	-16(%ebp),%edx
	movl	-12(%ebp),%ecx
	movl	16(%ebp),%ebx
	movl	%edx,(%ebx)
	movl	%ecx,4(%ebx)
	fldl	8(%ebp)
	fsubl	-16(%ebp)
	leal	-20(%ebp),%esp
	fclex
	fldcw	-4(%ebp)
	fwait
	popl	%ebx
	leave
	ret
