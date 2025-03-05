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

/*	Routines that implements threads (creation, end, dump, and AS
	assignment	*/

#include <ll/i386/stdio.h>
#include <ll/i386/mem.h>
#include <ll/i386/cons.h>
#include <ll/i386/error.h>

#include <ll/i386/tss-ctx.h>
#include <ll/sys/ll/ll-instr.h>			/* Only for HW instr... */
#include <ll/sys/ll/ll-func.h>

FILE(Context-Switch);

extern TSS  TSS_table[TSSMax];
extern WORD TSS_control[TSSMax];
extern BYTE ll_FPU_stdctx[FPU_CONTEXT_SIZE];

#ifdef __VIRCSW__
extern unsigned short int currCtx;
extern int activeInt;
#endif

extern DWORD entrypoint;

/* Just a debugging function; it dumps the status of the TSS */
void dump_TSS(WORD sel)
{
    BYTE acc,gran;
    DWORD base,lim;
    message("TR %x\n", sel);
    sel = TSSsel2index(sel);
    message("SS:SP %x:%lx\n", TSS_table[sel].ss, TSS_table[sel].esp);
    message("Stack0 : %x:%lx\n", TSS_table[sel].ss0, TSS_table[sel].esp0);
    message("Stack1 : %x:%lx\n", TSS_table[sel].ss1, TSS_table[sel].esp1);
    message("Stack2 : %x:%lx\n", TSS_table[sel].ss2, TSS_table[sel].esp2);
    message("CS : %x DS : %x\n", TSS_table[sel].cs, TSS_table[sel].ds);
    sel = TSSindex2sel(sel);
    message("Descriptor [%x] Info",sel);
    base = GDT_read(sel,&lim,&acc,&gran);
    message("Base : %lx Lim : %lx Acc : %x Gran %x\n", base, lim, (unsigned)(acc),(unsigned)(gran));
}


void ll_context_setspace(CONTEXT c, WORD as)
{
	int index = TSSsel2index(c);

    TSS_table[index].ss0 = as;
    TSS_table[index].cs = as + 8;
    TSS_table[index].es = as;
    TSS_table[index].ds = as;
    TSS_table[index].ss = as;
}

/* Initialize context -> TSS in 32 bit */
CONTEXT ll_context_create(void (*task)(void *p),BYTE *stack,
				void *parm,void (*killer)(void),WORD control)
{
    CONTEXT index = 0;
    DWORD *stack_ptr = (DWORD *)stack;

    /* Push onto stack the input parameter              */
    /* And the entry point to the task killer procedure */
    stack_ptr--;
    *stack_ptr = (DWORD)(parm);
    stack_ptr--;
    *stack_ptr = (DWORD)(killer);
    /* Find a free TSS */
    while ((TSS_control[index] & TSS_USED) && (index < TSSMain)) index++;
    /* This exception would signal an error */
    if (index >= TSSMain) {
    	message("No more Descriptors...\n");
	return 0;
    }
    TSS_control[index] |= (TSS_USED | control);
    /* Fill the TSS structure */
    /* No explicit protection; use only one stack */
    TSS_table[index].esp0 = (DWORD)(stack_ptr);
    TSS_table[index].esp1 = 0;
    TSS_table[index].esp2 = 0;
    TSS_table[index].ss0 = get_DS();
    TSS_table[index].ss1 = 0;
    TSS_table[index].ss2 = 0;
    /* No paging activated */
    TSS_table[index].cr3 = 0;
    /* Task entry point */
    TSS_table[index].eip = (DWORD)(task);
    /* Need only to unmask interrupts */
    TSS_table[index].eflags = 0x00000200;
    TSS_table[index].eax = 0;
    TSS_table[index].ebx = 0;
    TSS_table[index].ecx = 0;
    TSS_table[index].edx = 0;
    TSS_table[index].esi = 0;
    TSS_table[index].edi = 0;
    TSS_table[index].esp = (DWORD)(stack_ptr);
    TSS_table[index].ebp = (DWORD)(stack_ptr);
    TSS_table[index].cs = get_CS();
    TSS_table[index].es = get_DS();
    TSS_table[index].ds = get_DS();
    TSS_table[index].ss = get_DS();
    TSS_table[index].gs = X_FLATDATA_SEL;
    TSS_table[index].fs = X_FLATDATA_SEL;
    /* Use only the GDT */
    TSS_table[index].ldt = 0;
    TSS_table[index].trap = 0;
    TSS_table[index].io_base = 0;
    /* Fill in the coprocessor status */
    memcpy(TSS_table[index].ctx_FPU,ll_FPU_stdctx,FPU_CONTEXT_SIZE);
    /* Convert the index into a valid selector  */
    /* Which really represent CONTEXT           */
    return(TSSindex2sel(index));
}


/* Release the used TSS */

void ll_context_delete(CONTEXT c)
{
    int index = TSSsel2index(c);
    TSS_control[index] = 0;
}

#if 0		/* It does not work... Fix it!!! */
DWORD ll_push_func(void (*func)(void))
{
	DWORD *p;
	DWORD old;

	p = (DWORD *)entrypoint;
	old = *p;

	*p = (DWORD)func;
	
	return old;
}
#endif

/* Well this is used for debug purposes mainly; as the context is an       */
/* abstract type, we must provide some function to convert in into a       */
/* printable form; anyway this depend from the architecture as the context */

char *ll_context_sprintf(char *str, CONTEXT c)
{
    ksprintf(str, "%x (Hex)", c);
    return(str);
}
