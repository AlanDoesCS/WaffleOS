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

/* These function provide access to hardware structutes GDT/IDT */

#include <ll/i386/cons.h>
#include <ll/i386/mem.h>
#include <ll/math.h>

FILE(X-SysTab);

extern GATE IDT[256];

/* We usually need to set-up an interrupt handler; to do this we have */
/* to fill a GATE structure & copy it into the Interrupt Descriptor   */
/* Table or IDT; its phisical address is given by IDT_base	      */

void IDT_place(BYTE num,void (*handler)(void))
{
    DWORD offset = (DWORD)(handler);
    IDT[num].sel = X_FLATCODE_SEL;
    /* Set DPL = 3, to allow execution of the gate from any ring */
    IDT[num].access = INT_GATE386 | 0x60;
    IDT[num].dword_cnt = 0;
    IDT[num].offset_lo = offset & 0xFFFF;
    offset >>= 16;
    IDT[num].offset_hi = offset & 0xFFFF;
}

/* Ok; the GDT is managed in the same way as the IDT		    */
/* When a descriptor is cleared using it will cause a SEGMENT FAULT */
/* to refer such descriptor					    */

void GDT_place(WORD sel,DWORD base,DWORD lim,BYTE acc,BYTE gran)
{
    union gdt_entry x;
    /* This is declared in [wc32/gnu]\x0.[asm/s] */
    extern LIN_ADDR GDT_base;
/*    DWORD offset = appl2linear(&x); */
    x.d.base_lo = (base & 0x0000FFFF);
    x.d.base_med = ((base & 0x00FF0000) >> 16);
    x.d.base_hi = ((base & 0xFF000000) >> 24);
    x.d.access = acc;
    x.d.lim_lo = (lim & 0xFFFF);
    x.d.gran = (gran | ((lim >> 16) & 0x0F) | 0x40);    
    memcpy(GDT_base+(sel & ~3),&x,sizeof(union gdt_entry));
}

/* This function is used to read & format the descriptor data */
/* Anyway is better to use the hw 386 instruction to modify   */
/* a descriptor rather than reading,modifying & updating with */
/* this high level functions!				      */

DWORD GDT_read(WORD sel,DWORD *lim,BYTE *acc,BYTE *gran)
{
    union gdt_entry x;
    /* This is declared in [wc32/gnu]\x0.[asm/s] */
    extern LIN_ADDR GDT_base;
    /*DWORD offset = appl2linear(&x);*/
    DWORD base;
    memcpy(&x,GDT_base+sel,sizeof(union gdt_entry));
    base = x.d.base_hi;
    base <<= 8;
    base |= x.d.base_med;
    base <<= 16;
    base |= x.d.base_lo;
    if (lim != NULL) {
	*lim = (x.d.gran & 0x0F);
	*lim <<= 16;
	*lim |= x.d.lim_lo;
    }
    if (acc != NULL) *acc = x.d.access;
    if (gran != NULL) *gran = x.d.gran & 0xF0;
    return(base);
}
