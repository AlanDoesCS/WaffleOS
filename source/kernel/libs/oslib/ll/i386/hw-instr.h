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

/*	As the name says... All the hardware-dependent instructions
	there is a 1->1 corrispondence with ASM instructions	*/

#ifndef __LL_I386_HW_INSTR_H__
#define __LL_I386_HW_INSTR_H__

#include <ll/i386/defs.h>
BEGIN_DEF

#define INLINE_OP __inline__ static

#include <ll/i386/hw-data.h>

/* Low Level I/O funcs are in a separate file (by Luca) */
#include <ll/i386/hw-io.h>

INLINE_OP WORD get_CS(void)
{WORD r; __asm__ __volatile__ ("movw %%cs,%0" : "=q" (r)); return(r);}

INLINE_OP WORD get_DS(void)
{WORD r; __asm__ __volatile__ ("movw %%ds,%0" : "=q" (r)); return(r);}
INLINE_OP WORD get_FS(void)
{WORD r; __asm__ __volatile__ ("movw %%fs,%0" : "=q" (r)); return(r);}

/*INLINE_OP DWORD get_SP(void)
{DWORD r; __asm__ __volatile__ ("movw %%esp,%0" : "=q" (r)); return(r);}*/
INLINE_OP DWORD get_SP(void)
{
    DWORD rv;
    __asm__ __volatile__ ("movl %%esp, %0"
	  : "=a" (rv));
    return(rv);
}

INLINE_OP DWORD get_BP(void)
{
    DWORD rv;
    __asm__ __volatile__ ("movl %%ebp, %0"
	  : "=a" (rv));
    return(rv);
}

INLINE_OP WORD get_TR(void)
{WORD r; __asm__ __volatile__ ("strw %0" : "=q" (r)); return(r); }

INLINE_OP void set_TR(WORD n)
{__asm__ __volatile__("ltr %%ax": /* no output */ :"a" (n)); }

INLINE_OP void set_LDTR(WORD addr)
{ __asm__ __volatile__("lldt %%ax": /* no output */ :"a" (addr)); }


/* Clear Task Switched Flag! Used for FPU preemtion */
INLINE_OP void clts(void)
{__asm__ __volatile__ ("clts"); }

/* Halt the processor! */
INLINE_OP void hlt(void)
{__asm__ __volatile__ ("hlt"); }

/* These functions are used to mask/unmask interrupts           */
INLINE_OP void sti(void) {__asm__ __volatile__ ("sti"); }
INLINE_OP void cli(void) {__asm__ __volatile__ ("cli"); }

INLINE_OP SYS_FLAGS ll_fsave(void)
{
    SYS_FLAGS result;
    
    __asm__ __volatile__ ("pushfl");
    __asm__ __volatile__ ("cli");
    __asm__ __volatile__ ("popl %eax");
    __asm__ __volatile__ ("movl %%eax,%0"
	: "=r" (result)
	:
	: "eax" );
    return(result);
}

INLINE_OP void ll_frestore(SYS_FLAGS f)
{
    __asm__ __volatile__ ("mov %0,%%eax"
	:
	: "r" (f)
	: "eax");
    __asm__ __volatile__ ("pushl %eax");
    __asm__ __volatile__ ("popfl");
}

/*
    FPU context switch management functions!
    FPU management exported at kernel layer to allow the use
    of floating point in kernel primitives; this turns to be
    useful for bandwidth reservation or guarantee!
*/

/* FPU lazy state save handling.. */
INLINE_OP void save_fpu(TSS *t)
{
    __asm__ __volatile__("fnsave %0\n\tfwait":"=m" (t->ctx_FPU));
}

INLINE_OP void restore_fpu(TSS *t)
{
#if 1
    __asm__ __volatile__("frstor %0": :"m" (t->ctx_FPU));
#else
    __asm__ __volatile__("frstor %0\n\tfwait": :"m" (t->ctx_FPU));
#endif
/*    __asm__ __volatile__("frstor _LL_FPU_savearea"); */
}

INLINE_OP void smartsave_fpu(TSS *t)
{
    if (t->control & FPU_USED) save_fpu(t);
}

INLINE_OP void reset_fpu(void) { __asm__ __volatile__ ("fninit"); }

INLINE_OP int check_fpu(void)
{
    int result;
    
    __asm__ __volatile__ ("movl %cr0,%eax");
    __asm__ __volatile__ ("movl %eax,%edi");
    __asm__ __volatile__ ("andl $0x0FFFFFFEF,%eax");
    __asm__ __volatile__ ("movl %eax,%cr0");
    __asm__ __volatile__ ("movl %cr0,%eax");
    __asm__ __volatile__ ("xchgl %edi,%eax");
    __asm__ __volatile__ ("movl %eax,%cr0");
    __asm__ __volatile__ ("xorl %eax,%eax");
    __asm__ __volatile__ ("movb %bl,%al");
    __asm__ __volatile__ ("shrb $4,%al");
    __asm__ __volatile__ ("movl %%eax,%0"
	: "=r" (result)
	:
	: "eax" );
    return(result);
}

INLINE_OP void init_fpu(void)
{
    __asm__ __volatile__ ("movl %cr0,%eax");
    __asm__ __volatile__ ("orl  $34,%eax");
    __asm__ __volatile__ ("movl %eax,%cr0");
    __asm__ __volatile__ ("fninit");
}


extern BYTE LL_FPU_savearea[];

extern __inline__ void LL_FPU_save(void)
{
    #ifdef __LINUX__
	__asm__ __volatile__ ("fsave LL_FPU_savearea");
    #else
	__asm__ __volatile__ ("fsave _LL_FPU_savearea");
    #endif
}

extern __inline__ void LL_FPU_restore(void)
{
    #ifdef __LINUX__
	__asm__ __volatile__ ("frstor LL_FPU_savearea");
    #else
	__asm__ __volatile__ ("frstor _LL_FPU_savearea");
    #endif
}




    
INLINE_OP void lmempokeb(LIN_ADDR a, BYTE v)
{
	*((BYTE *)a) = v;
}
INLINE_OP void lmempokew(LIN_ADDR a, WORD v)
{
	*((WORD *)a) = v;
}
INLINE_OP void lmempoked(LIN_ADDR a, DWORD v)
{
	*((DWORD *)a) = v;
}

INLINE_OP BYTE lmempeekb(LIN_ADDR a)
{
	return *((BYTE *)a);
}
INLINE_OP WORD lmempeekw(LIN_ADDR a)
{
	return *((WORD *)a);
}
INLINE_OP DWORD lmempeekd(LIN_ADDR a)
{
	return *((DWORD *)a);
}

END_DEF

#endif
