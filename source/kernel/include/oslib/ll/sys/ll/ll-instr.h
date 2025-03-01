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

#ifndef __LL_SYS_LL_LL_INSTR_H_
#define __LL_SYS_LL_LL_INSTR_H_

#include <ll/i386/defs.h>
BEGIN_DEF

#include <ll/i386/hw-instr.h>
#include <ll/i386/hw-func.h>
/*
    Well, these are simple macros... to map the HARTIK names
    onto the standard names!
 */
#define ll_in(port)	inp(port)
#define ll_out(port,v)	outp(port,v)
#define ll_inw(port)	inpw(port)
#define ll_outw(port,v)	outpw(port,v)
#define ll_ind(port)	inpd(port)
#define ll_outd(port,v)	outpd(port,v)



/* These functions are used to mask/unmask selectively interrupts    */
/* The irq services are also #defined to allow more generic inteface */
/* This is done into hw... files!				     */

void ll_irq_mask(WORD irqno);
void ll_irq_unmask(WORD irqno);

/* These functions provide direct access to interrupt table 	*/
/* We can write the HARTIK interrupt table but only read	*/
/* the host OS interrupt table!					*/
void ll_irq_set(WORD irqno,INTERRUPT handler);
INTERRUPT ll_irq_get(WORD irqno);

END_DEF
#endif
