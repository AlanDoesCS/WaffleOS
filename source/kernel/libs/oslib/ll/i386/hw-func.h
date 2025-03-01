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

/*	As the name says...
	All the hardware-dependent functions
		IDT/GDT management
		context switch
		IRQ/Exc handling...	*/

#ifndef __LL_I386_HW_FUNC_H__
#define __LL_I386_HW_FUNC_H__

#include <ll/i386/defs.h>
BEGIN_DEF

#include <ll/i386/hw-instr.h>

/* Low level exit functions! It will halt, reboot or
 * give back the control to X, depending on how YAMME started...
 */
void _exit(int code) __attribute__((noreturn));
void halt(void);

/* Functions to reboot the machine */
void cold_reboot(void);
void warm_reboot(void);
void reboot(int mode);

/* System tables management functions */
void IDT_place(BYTE num,void (*handler)(void));
void GDT_place(WORD sel,DWORD base,DWORD lim,BYTE acc,BYTE gran);
DWORD GDT_read(WORD sel,DWORD *lim,BYTE *acc,BYTE *gran);
LIN_ADDR addr2linear(unsigned short sel,unsigned long offset);

/* These 3 function realize the context switching. The context_save has   */
/* to be the first call of a kernel primitive, and the context_change has */
/* to be the last call.							  */
/* The context_save disables the interrupt (a kernel primitive must be	  */
/* atomic) and return the context of the running task.			  */
/* The context_change take the context of the new task (or of the	  */
/* same task), switch to the new context and return restoring the flag	  */
/* register.								  */
/* The context_load is used when the task is going to be killed; then its */
/* context does not need to be saved; we only need to load the context of */
/* the new task; the effective implementation of this functions can vary  */
/* greatly throughout different implementations as some of them are       */
/* mapped to empty functions or mapped one onto another			  */

CONTEXT ll_context_save(void);
void    ll_context_change(CONTEXT c);
void    ll_context_load(CONTEXT c);

CONTEXT ll_context_from(void);
void    ll_context_to(CONTEXT c);



void *x_init(void);
void x_end(void);
void x_exc_bind(int i, void (*f)(int n));
void x_irq_bind(int i, void (*f)(int n));


END_DEF

#endif
