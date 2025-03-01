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

/* File: Vm86.C
 *	   			
 * VM86 mode switch routines!
 * This is basically an alternative way of invoking the
 * BIOS service routines; it is very useful to support
 * native VBE compliant Video card, without writing an explicit driver
 */

#include <ll/i386/mem.h>
#include <ll/i386/x-bios.h>
#include <ll/i386/x-dosmem.h>
#include <ll/i386/hw-func.h>
#include <ll/i386/hw-instr.h>
#include <ll/i386/cons.h>
#include <ll/i386/error.h>

FILE(VM-86);

/*
#define __LL_DEBUG__
#define __DUMB_CODE__
#define __CHK_IO__
*/

#define VM86_STACK_SIZE 1024 

/* TSS optional section */
static BYTE  vm86_stack0[VM86_STACK_SIZE];

static struct {
    TSS t;
    DWORD io_map[2048];
} vm86_TSS;
static LIN_ADDR vm86_stack;
static LIN_ADDR vm86_iretAddress;


DWORD *GLOBesp;


#ifdef __DUMB_CODE__
static LIN_ADDR vm86_code;
static BYTE prova86[] = {
	                0x1e,			/* push ds		*/
	                0xb8,0x00,0xb8,         /* mov ax,0xb800	*/
                        0x8e,0xd8,              /* mov ds,ax		*/
                        0xbf,0x9e,0x00,         /* mov di,0x009e (158)	*/
                        0xb0,0x2a,         	/* mov ax,'*'   	*/
                        0x88,0x05,         	/* mov ds:[di],al	*/
                        0x1f,			/* pop ds		*/
			0xcd, 0x40,		/*???*/
#ifdef __CHK_IO__		
			0xb0, 0x00,             /* movb   $0x0,%al*/
			0x66,0xba, 0x80, 0x00, 	/* movw   $0x80,%dx */
			0x66,0xef,   		/* outw %ax, (%dx) */
#endif		
			0xcf,                   /* iret			*/
                        0xf4,                   /* hlt			*/
                        0};    
#endif

#ifdef __LL_DEBUG__
static BYTE vm86_retAddr[] = {
	                0x1e,			/* push ds		*/
	                0xb8,0x00,0xb8,         /* mov ax,0xb800	*/
                        0x8e,0xd8,              /* mov ds,ax		*/
                        0xbf,0x3e,0x01,         /* mov di,0x013c (316)	*/
                        0xb0,'%',         	/* mov ax,'%'		*/
                        0x88,0x05,         	/* mov ds:[di],al	*/
                        0x1f,			/* pop ds		*/
			0xcd, 0x48};		/* int 0x48		*/
#else
static BYTE vm86_retAddr[] = { 0xcd, 0x48};	/* int 48h		*/
#endif

/* This is the return point from V86 mode, called through int 0x48 
 * (see vm86-exc.s). We double check that this function is called in
 * the V86 TSS. Otherwise, Panic!!!
 */
void vm86_return(DWORD *tos)
{
    CONTEXT c = get_TR();
#ifdef __LL_DEBUG__
    DWORD cs,eip;
    void *esp;
    DWORD a;
/*    message("Gotta code=%d [0 called from GPF/1 int 0x48]\n",code);*/
#endif
    if (c == X_VM86_TSS) {
	GLOBesp = tos;
#ifdef __LL_DEBUG__
	message("TSS CS=%x IP=%lx\n",vm86_TSS.t.cs,vm86_TSS.t.eip);
        message("Switching to %x\n", vm86_TSS.t.back_link);
	a = (DWORD)(vm86_iretAddress);
    	cs = (a & 0xFF000) >> 4;
    	eip = (a & 0xFFF);
	message("Real-Mode Address is CS=%lx IP=%lx\nLinear=%lx\n",cs,eip,a);
	esp = (void *)(tos);
	message("Stack frame: %p %lx %lx\n", 
			esp, vm86_TSS.t.esp0, vm86_TSS.t.esp);
	message("%lx ",lmempeekd(esp));		/* bp */
	message("%lx ",lmempeekd(esp+4));	/* eip */
	message("%lx ",lmempeekd(esp+8));	/* 0x0d */
	message("%lx\n",lmempeekd(esp+12));	/* error code */
/* The error code is given by the selector causing shifted and or-ed with
   3 bits: [LDT/GDT | IDT | Ext/Int]
   If IDT == 1 -> the fault was provoked bu an interrupt (Internal if the
   Ext/Int bit is 0, External if the bit is 1)
   Else the LDT/GDT bit shows if the selector belongs to the LDT (if 1)
   or GDT (if 0)  
*/
	message("%lx ",lmempeekd(esp+16));	/* EIP of faulting instr */
	message("%lx ",lmempeekd(esp+20));	/* CS of faulting instr*/
	message("%lx ",lmempeekd(esp+24));	/* EFLAGS*/
	message("%lx\n",lmempeekd(esp+28));	/* old ESP*/
	message("%lx ",lmempeekd(esp+32));	/* old SS*/
	message("%lx ",lmempeekd(esp+36));	/* old ES*/
	message("%lx ",lmempeekd(esp+40));	/* old DS*/
	message("%lx\n",lmempeekd(esp+44));	/* old FS*/
#endif
        ll_context_load(vm86_TSS.t.back_link);
    }
    halt();
}

extern void vm86_exc(void);

/* Just a debugging function; it dumps the status of the TSS */
void vm86_dump_TSS(void)
{
    BYTE acc,gran;
    DWORD base,lim;
    message("vm86_TSS.t dump\n");
    message("Flag: %lx\n",vm86_TSS.t.eflags);
    message("SS: %hx SP:%lx\n", vm86_TSS.t.ss,vm86_TSS.t.esp);
    message("Stack0: %hx:%lx\n",vm86_TSS.t.ss0,vm86_TSS.t.esp0);
    message("Stack1: %hx:%lx\n",vm86_TSS.t.ss1,vm86_TSS.t.esp1);
    message("Stack2: %hx:%lx\n",vm86_TSS.t.ss2,vm86_TSS.t.esp2);
    message("CS: %hx IP: %lx",vm86_TSS.t.cs, vm86_TSS.t.eip);
    message("DS: %hx\n",vm86_TSS.t.ds);
    base = GDT_read(X_VM86_TSS,&lim,&acc,&gran);
    message("Base : %lx Lim : %lx Acc : %x Gran %x\n",
		    base,lim,(unsigned)(acc),(unsigned)(gran));
}

void vm86_init(void)
{
    int register i;
    /* First of all, we need to setup a GDT entries to
     * allow vm86 task execution. We just need a free 386 TSS, which
     * will be used to store the execution context of the virtual 8086
     * task
     */
    GDT_place(X_VM86_TSS,(DWORD)(&vm86_TSS),
              sizeof(vm86_TSS),FREE_TSS386,GRAN_16);
    IDT_place(0x48,vm86_exc);

    /* Prepare a real-mode stack, obtaining it from the
     * DOS memory allocator!
     * 8K should be OK! Stack top is vm86_stack + SIZE!
     */
    vm86_stack = DOS_alloc(VM86_STACK_SIZE*2);
    vm86_stack += VM86_STACK_SIZE/2;
    
    /* Create a location of DOS memory containing the 
     * opcode sequence which will generate a GPF 
     * We use the privileged instruction hlt to do it
     */
    vm86_iretAddress = DOS_alloc(sizeof(vm86_retAddr));
    memcpy(vm86_iretAddress,vm86_retAddr,sizeof(vm86_retAddr));
#ifdef __LL_DEBUG__
    message("PM reentry linear address=%p\n",vm86_iretAddress);
#endif
#ifdef __DUMB_CODE__
    vm86_code = DOS_alloc(2048);
    lmemcpy(vm86_code,prova86,sizeof(prova86));
#endif
    /* Zero the PM/Ring[1,2] ss:esp; they're unused! */
    vm86_TSS.t.esp1 = 0;
    vm86_TSS.t.esp2 = 0;
    vm86_TSS.t.ss1 = 0;
    vm86_TSS.t.ss2 = 0;
    /* Use only the GDT */
    vm86_TSS.t.ldt = 0;
    /* No paging activated */
    vm86_TSS.t.cr3 = 0;
    vm86_TSS.t.trap = 0;
    /* Yeah, free access to any I/O port; we trust BIOS anyway! */
    /* Here is the explanation: we have 65536 I/O ports... each bit
     * in the io_map masks/unmasks the exception for the given I/O port
     * If the bit is set, an exception is generated; otherwise, if the bit
     * is clear, everythings works fine...
     * Because of alignment problem, we need to add an extra byte all set
     * to 1, according to Intel manuals
     */
    vm86_TSS.t.io_base = (DWORD)(&(vm86_TSS.io_map)) -
	    		(DWORD)(&(vm86_TSS));
    for (i = 0; i < 2047; i++) vm86_TSS.io_map[i] = 0;
    vm86_TSS.io_map[2047] = 0xFF000000;
}

int vm86_callBIOS(int service,X_REGS16 *in,X_REGS16 *out,X_SREGS16 *s)
{
    DWORD vm86_tmpAddr;
    DWORD vm86_flags, vm86_cs,vm86_ip;
    LIN_ADDR vm86_stackPtr;
    DWORD *IRQTable_entry;
    
    if (service < 0x10 || in == NULL) return -1;
    /* Setup the stack frame */
    vm86_tmpAddr = (DWORD)(vm86_stack);
    vm86_TSS.t.ss = (vm86_tmpAddr & 0xFF000) >> 4;
    vm86_TSS.t.ebp = vm86_TSS.t.esp = (vm86_tmpAddr & 0x0FFF)
	    	+ VM86_STACK_SIZE - 6;
    /* Build an iret stack frame which returns to vm86_iretAddress */
    vm86_tmpAddr = (DWORD)(vm86_iretAddress);
    vm86_cs = (vm86_tmpAddr & 0xFF000) >> 4;
    vm86_ip = (vm86_tmpAddr & 0xFFF);
    vm86_flags = 0; /* CPU_FLAG_VM | CPU_FLAG_IOPL; */
    vm86_stackPtr = vm86_stack + VM86_STACK_SIZE;
    lmempokew(vm86_stackPtr-6,vm86_ip);
    lmempokew(vm86_stackPtr-4,vm86_cs);
    lmempokew(vm86_stackPtr-2,vm86_flags);
#ifdef __LL_DEBUG__
    message("Stack: %lx SS: %lx SP: %lx\n",
	vm86_tmpAddr + VM86_STACK_SIZE,(DWORD)vm86_TSS.t.ss,vm86_TSS.t.esp);
#endif
    /* Wanted VM86 mode + IOPL = 3! */
    vm86_TSS.t.eflags = CPU_FLAG_VM | CPU_FLAG_IOPL;
    /* Preload some standard values into the registers */
    vm86_TSS.t.ss0 = X_FLATDATA_SEL;
    vm86_TSS.t.esp0 = (DWORD)&(vm86_stack0[VM86_STACK_SIZE-1]); 
    
    
#ifdef __DUMB_CODE__
    vm86_TSS.t.cs = ((DWORD)(vm86_code) & 0xFFFF0) >> 4;
    vm86_TSS.t.eip = ((DWORD)(vm86_code) & 0x000F);
#ifdef __LL_DEBUG_
    message("(DUMB CODE) CS:%x IP:%x/%x\n",
        (DWORD)vm86_TSS.t.cs,vm86_TSS.t.eip,&prova86);
    message("(DUMB CODE) Go...\n");
#endif
    vm86_TSS.t.back_link = ll_context_save();
    ll_context_load(X_VM86_TSS);

#ifdef __LL_DEBUG_
    message("(DUMB CODE) I am back...\n");
#endif
#else
    /* Copy the parms from the X_*REGS structures in the vm86 TSS */
    vm86_TSS.t.eax = (DWORD)in->x.ax;
    vm86_TSS.t.ebx = (DWORD)in->x.bx;
    vm86_TSS.t.ecx = (DWORD)in->x.cx;
    vm86_TSS.t.edx = (DWORD)in->x.dx;
    vm86_TSS.t.esi = (DWORD)in->x.si;
    vm86_TSS.t.edi = (DWORD)in->x.di;
    /* IF Segment registers are required, copy them... */
    if (s != NULL) {
        vm86_TSS.t.es = (WORD)s->es;
        vm86_TSS.t.ds = (WORD)s->ds;
    } else {
        vm86_TSS.t.ds = vm86_TSS.t.ss;
        vm86_TSS.t.es = vm86_TSS.t.ss; 
    }
    vm86_TSS.t.gs = vm86_TSS.t.ss; 
    vm86_TSS.t.fs = vm86_TSS.t.ss; 
    /* Execute the BIOS call, fetching the CS:IP of the real interrupt
     * handler from 0:0 (DOS irq table!)
     */
    IRQTable_entry = (void *)(0L);
    vm86_TSS.t.cs= ((IRQTable_entry[service]) & 0xFFFF0000) >> 16;
    vm86_TSS.t.eip = ((IRQTable_entry[service]) & 0x0000FFFF);
#ifdef __LL_DEBUG__    
    message("CS:%hx IP:%lx\n", vm86_TSS.t.cs, vm86_TSS.t.eip);
#endif
    /* Let's use the ll standard call... */
    vm86_TSS.t.back_link = ll_context_save();
    ll_context_load(X_VM86_TSS);
#ifdef __LL_DEBUG__    
    message("I am back...\n");
    message("TSS CS=%hx IP=%lx\n", vm86_TSS.t.cs, vm86_TSS.t.eip);
    { char *xp = (char *)(vm86_iretAddress + 0xe);
      message("PM reentry linear address=%p\n", vm86_iretAddress);
      message("Executing code: %x ",(unsigned char)(*xp)); xp++;
      message("%x\n",(unsigned char)(*xp));}
#endif
    /* Send back in the X_*REGS structure the value obtained with
     * the real-mode interrupt call
     */
    if (out != NULL) {
/*
      out->x.ax = (WORD)vm86_TSS.t.eax;
      out->x.bx = (WORD)vm86_TSS.t.ebx;
      out->x.cx = (WORD)vm86_TSS.t.ecx;
      out->x.dx = (WORD)vm86_TSS.t.edx;
      out->x.si = (WORD)vm86_TSS.t.esi;
      out->x.di = (WORD)vm86_TSS.t.edi;
      out->x.cflag = (WORD)vm86_TSS.t.eflags;
*/
#ifdef __LL_DEBUG__
	message("%x\n", (WORD)*(GLOBesp));
	message("%x\n", (WORD)*(GLOBesp+1)); /*EDI*/
	message("%x\n", (WORD)*(GLOBesp+2)); /*ESI*/
	message("%x\n", (WORD)*(GLOBesp+3)); /*EBP*/
	message("%x\n", (WORD)*(GLOBesp+4)); /*ESP*/
	message("%x\n", (WORD)*(GLOBesp+5)); /*EBX*/
	message("%x\n", (WORD)*(GLOBesp+6)); /*EDX*/
#endif
      out->x.ax = (WORD)*(GLOBesp + 8);
      out->x.bx = (WORD)*(GLOBesp + 5);
      out->x.cx = (WORD)*(GLOBesp + 7);
      out->x.dx = (WORD)*(GLOBesp + 6);
      out->x.si = (WORD)*(GLOBesp + 2);
      out->x.di = (WORD)*(GLOBesp + 1);
      out->x.cflag = (WORD)*(GLOBesp);
    }
    if (s != NULL) {
      s->es = vm86_TSS.t.es;
      s->ds = vm86_TSS.t.ds;
    }
#endif
    return 1;
}
