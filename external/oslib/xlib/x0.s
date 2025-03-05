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

/* The first things to do when an OSLib application starts :	*/
/* Set up segment registers & stack; then execute startup code	*/
/* When the application returns the gate-jmp make us return to	*/
/* RM through X interface!					*/

/* Use X standard GDT selectors */
#include <ll/i386/sel.h>
#include <ll/i386/linkage.h>
#include <ll/i386/defs.h>
#include <ll/i386/mb-hdr.h>

/* #define __DEBUG__ */

#ifdef __LINUX__ /* ELF mode */
#define MULTIBOOT_FLAGS (MULTIBOOT_MEMORY_INFO)
#else /* else it is COFF! */
#define MULTIBOOT_FLAGS (MULTIBOOT_MEMORY_INFO | MULTIBOOT_AOUT_KLUDGE)
#endif

.extern SYMBOL_NAME(_startup)
.extern SYMBOL_NAME(_stkbase)
.extern SYMBOL_NAME(_stktop)

.extern SYMBOL_NAME(halt)

.data

ASMFILE(X0)

.globl SYMBOL_NAME(IDT)
.globl SYMBOL_NAME(GDT_base)
.globl SYMBOL_NAME(mb_signature)
.globl SYMBOL_NAME(mbi)

/* GDT Definition */
GDT:
.word	0,0,0,0		/* X_NULL_SEL */
.word	0,0,0,0		/* X_DATA16_SEL */
.word	0,0,0,0		/* X_CODE16_SEL */
.word	0,0,0,0		/* X_CODE32_SEL */
			/* X_RM_BACK_GATE */
rmBckGateFix1:		/* X_FLATCODE_SEL */
.word	0
.word	X_FLATCODE_SEL	
.word	0x8C00
rmBckGateFix2:
.word	0

.word	0,0,0,0		/* X_PM_BACK_GATE */
.word	0xFFFF		/* X_FLATDATA_SEL */ 
.word	0
.word	0x9200
.word	0x00CF
.word	0xFFFF		/* X_FLATCODE_SEL */ 
.word	0
.word	0x9A00
.word	0x00CF
.word	0,0,0,0		/* X_CALLBIOS_SEL */
.word	0,0,0,0		/* X_CALLBIOS_GATE */
.word	0,0,0,0		/* X_VM86_TSS */
.word	0,0,0,0		/* X_MAIN_TSS */
	.fill 256 - 12,8,0
	
GDT_descr:
.word 256*8-1 
SYMBOL_NAME_LABEL(GDT_base) 
.long GDT

/* IDT definition */
SYMBOL_NAME_LABEL(IDT)
	.fill 256,8,0		# idt is uninitialized
IDT_descr:
	.word 256*8-1		# idt contains 256 entries
	.long SYMBOL_NAME(IDT)

/* MultiBoot Data Stuff definition */
SYMBOL_NAME_LABEL(mb_signature) .long 0
SYMBOL_NAME_LABEL(mbi) .long 0

/* Protected mode stack */
base:
.space 	8192,0
tos:

.text
.globl	SYMBOL_NAME(_start)
/*.globl	SYMBOL_NAME(start)*/
.globl start

SYMBOL_NAME_LABEL(_start)
/*SYMBOL_NAME_LABEL(start)*/
start:
.align		8
/*start:*/
		jmp	boot_entry		
		/* 
		Here we go with the multiboot header... 
		---------------------------------------
		*/		
.align		8
boot_hdr:
.align		8
		.long	MULTIBOOT_MAGIC
		.long   MULTIBOOT_FLAGS
		/* Checksum */
		.long	-(MULTIBOOT_MAGIC+MULTIBOOT_FLAGS)
#ifndef __LINUX__ /* COFF mode */
		.long	boot_hdr
/*		.long	SYMBOL_NAME(start)*/
		.long   start
		.long	_edata
		.long	_end
		.long	boot_entry
#endif 
				
boot_entry:	/* Just a brief debug check */	
		#ifdef __DEBUG__
		/* A Brown 1 should appear... */
		    movl	$0xB8000,%edi
		    addl	$158,%edi
		    movb	$'1',%gs:0(%edi)
		    incl	%edi
		    movb	$6,%gs:0(%edi)
		#endif
		
		/* 
		 * Hopefully if it gets here, CS & DS are
		 * Correctly set for FLAT LINEAR mode
		 */
		/* Test if GDT is usable */
		movl	%gs:0(%ebx),%ecx
		andl	$0x080,%ecx
		jnz	GDT_is_OK
		/* 
		 * Fix the X_RM_BACK_GATE with the address of halt()
		*/

/* Now I test if the check mechanism is OK... */
movl	$0xB8000,%edi
addl	$150,%edi
movb	$'1',%gs:0(%edi)
incl	%edi
movb	$6,%gs:0(%edi)
incl	%edi
movb	$'2',%gs:0(%edi)
incl	%edi
movb	$6,%gs:0(%edi)

		movl	$SYMBOL_NAME(halt),%eax
		movw	%ax,%gs:rmBckGateFix1
		shrl	$16,%eax
		movw	%ax,%gs:rmBckGateFix2
		
		/* Load GDT, using the predefined assignment! */
		lgdt	GDT_descr
movl	$0xB8000,%edi
addl	$146,%edi
movb	$'0',%gs:0(%edi)
incl	%edi
movb	$6,%gs:0(%edi)
		
GDT_is_OK:	movw	$(X_FLATDATA_SEL),%ax
		movw	%ax,%ds
		movw	%ax,%es
		movw	%ax,%ss
		movw	%ax,%fs
		movw	%ax,%gs
		movl	$tos,%esp
		movl	$base,SYMBOL_NAME(_stkbase)
		movl	$tos,SYMBOL_NAME(_stktop)
		
		/* Store the MULTIBOOT informations */
		movl	%eax,SYMBOL_NAME(mb_signature)
		movl	%ebx,SYMBOL_NAME(mbi)
		
		/* Store the X passed GDT address!
		 * If GDT is not available is a dummy instruction!
		 */
		sgdt 	GDT_descr

		/* Now probably is the case to load CS... */
		ljmp $X_FLATCODE_SEL, $load_cs

load_cs:
		/* Load IDT */
		lidt	IDT_descr
		
		cld
		call SYMBOL_NAME(_startup)
	
		/* Well I hope when I reach this
		   The X_RM_BACK_GATE has been setup correctly
		   even if the kernel has not been loaded through X!
		*/
		.byte	0x0EA		   /* Direct gate jmp */
		.long	0
		.word	X_RM_BACK_GATE

