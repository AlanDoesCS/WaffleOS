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

/* Virtual VM86 Mode swith return handler 			 */

.title	"vm86-exc.S"

#include <ll/i386/sel.h>
#include <ll/i386/linkage.h>
#include <ll/i386/defs.h>


#ifdef __LINUX__
# define P2ALIGN(p2)	.align	(1<<(p2))
#else
# define P2ALIGN(p2)	.align	p2
#endif
#ifdef __LINUX__
#define FUNCSYM(x)	.type    x,@function
#else
#define FUNCSYM(x)	/* nothing */
#endif

#define TEXT_ALIGN	4

.data

ASMFILE(VM86-Exc)

.text
		.globl SYMBOL_NAME(vm86_exc)

SYMBOL_NAME_LABEL(vm86_exc)
/*
			movl $0xAAAA, %eax
			movl $0xBBBB, %ebx
			movl $0xCCCC, %ecx
			movl $0xDDDD, %edx
*/
			pushal
			pushfl
			movw	$(X_FLATDATA_SEL),%ax
			movw	%ax,%es
			movw	%ax,%ds
/*
			movl	$0xB8020,%edi
			movl	$'*',%ds:(%edi)
			pushl	$1
*/
			cld
			pushl	%esp
			call	SYMBOL_NAME(vm86_return)
			addl	$4,%esp
			iret

