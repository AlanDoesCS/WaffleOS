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

/*	PIC management code & data	*/

#include <ll/i386/hw-instr.h>

FILE(IRQ);

#define ICW1_M 	0x020 /* Master PIC (8259) register settings */
#define ICW2_M  0x021
#define ICW3_M  0x021
#define ICW4_M  0x021
#define OCW1_M  0x021
#define OCW2_M  0x020
#define OCW3_M  0x020

#define ICW1_S  0x0A0 /* Slave PIC register setting */
#define ICW2_S  0x0A1
#define ICW3_S  0x0A1
#define ICW4_S  0x0A1
#define OCW1_S  0x0A1
#define OCW2_S  0x0A0
#define OCW3_S  0x0A0

#define PIC1_BASE 0x040 /* Interrupt base for each PIC in HARTIK 3.0 */
#define PIC2_BASE 0x070 
#define EOI       0x020 /* End Of Interrupt code for PIC! */

#define bit_on(v,b)     ((v) |= (1 << (b)))
#define bit_off(v,b)    ((v) &= ~(1 << (b)))

/* PIC interrupt mask */
BYTE ll_PIC_master_mask = 0xFE;
BYTE ll_PIC_slave_mask = 0xFE;


void PIC_init(void)
{
    outp(ICW1_M,0x11);
    outp(ICW2_M,PIC1_BASE);
    outp(ICW3_M,0x04);
    outp(ICW4_M,0x01);
    outp(OCW1_M,0xFF);

    outp(ICW1_S,0x11);
    outp(ICW2_S,PIC2_BASE);
    outp(ICW3_S,0x02);
    outp(ICW4_S,0x01);
    outp(OCW1_S,0xFF);
}

void PIC_end(void)
{
    outp(ICW1_M,0x11);
    outp(ICW2_M,0x08);
    outp(ICW3_M,0x04);
    outp(ICW4_M,0x01);
    outp(OCW1_M,0xFF);

    outp(ICW1_S,0x11);
    outp(ICW2_S,0x70);
    outp(ICW3_S,0x02);
    outp(ICW4_S,0x01);
    outp(OCW1_S,0xFF);
}

void irq_mask(WORD irqno)
{
    /* Cannot mask timer interrupt! */
    if (irqno == 0) return;
    /* Interrupt is on master PIC */
    if (irqno < 8) {
	bit_on(ll_PIC_master_mask,irqno);
	outp(0x21,ll_PIC_master_mask);
    } else if (irqno < 16) {
	/* Interrupt on slave PIC */
	bit_on(ll_PIC_slave_mask,irqno-8);
	outp(0xA1,ll_PIC_slave_mask);
	/* If the slave PIC is completely off   */
	/* Then turn off cascading line (Irq #2)*/
	if (ll_PIC_slave_mask == 0xFF && !(ll_PIC_master_mask & 0x04)) {
	    bit_on(ll_PIC_master_mask,2);
	    outp(0x21,ll_PIC_master_mask);
	}
    }
}

void irq_unmask(WORD irqno)
{
    /* It is a nonsense to unmask the timer interrupt */
    if (irqno == 0) return;
    /* Interrupt is on master PIC */
    if (irqno < 8) {
	bit_off(ll_PIC_master_mask,irqno);
	outp(0x21,ll_PIC_master_mask);
    } else if (irqno < 16) {
	/* Interrupt on slave PIC */
	bit_off(ll_PIC_slave_mask,irqno-8);
	outp(0xA1,ll_PIC_slave_mask);
	/* If the cascading irq line was off */
	/* Then activate it also!            */
	if (ll_PIC_master_mask & 0x04) {
	    bit_off(ll_PIC_master_mask,2);
	    outp(0x21,ll_PIC_master_mask);
	}
    }
}
