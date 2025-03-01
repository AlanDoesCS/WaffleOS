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

/*	Xlib initialization code	*/

#include <ll/i386/mem.h>
#include <ll/i386/cons.h>
#include <ll/i386/mb-info.h>
#include <ll/i386/error.h>
#include <ll/i386/pit.h>
#include <ll/i386/pic.h>

#include <ll/i386/tss-ctx.h>
#include <ll/i386/hw-arch.h>

FILE(X-Init);

extern DWORD ll_irq_table[16];
extern DWORD ll_exc_table[16];

/* Architecture definition */
LL_ARCH ll_arch;

/* These are declared in llCx32b.C */
TSS  TSS_table[TSSMax];
WORD TSS_control[TSSMax];
BYTE ll_FPU_stdctx[FPU_CONTEXT_SIZE];

/* The following stuff is in llCxA.Asm/S    */

/* Assembly external routines!      */
/* Setup the TR register of the 80386, to initialize context switch */

extern void init_TR(WORD v);

/* ll hardware interrupt hooks  */
extern void h1(void);
extern void h2(void);
extern void h3(void);
extern void h4(void);
extern void h5(void);
extern void h6(void);
extern void h7(void);
extern void h8(void);
extern void h9(void);
extern void h10(void);
extern void h11(void);
extern void h12(void);
extern void h13(void);
extern void h13_bis(void);
extern void h14(void);
extern void h15(void);

/* ll hardware exception hooks  */
/* In llCtx1.Asm/s              */
extern void exc0(void);
extern void exc1(void);
extern void exc2(void);
extern void exc3(void);
extern void exc4(void);
extern void exc5(void);
extern void exc6(void);
extern void exc7(void);
extern void exc8(void);
extern void exc9(void);
extern void exc10(void);
extern void exc11(void);
extern void exc12(void);
extern void exc13(void);
extern void exc14(void);
extern void exc15(void);
extern void exc16(void);

static void dummyfun(int i)
{
	message("Unhandled Exc or Int %d occured!!!\n", i);
	halt();
}

void x_exc_bind(int i, void (*f)(int n))
{
	ll_exc_table[i] = (DWORD)f;
}

void x_irq_bind(int i, void (*f)(int n))
{
	ll_irq_table[i] = (DWORD)f;
}








void *x_init(void)
{
	register int i;
	struct ll_cpuInfo cpuInfo;
	LIN_ADDR b;
	extern BYTE X86_fpu;

	TSS dummy_tss;      /* Very dirty, but we need it, in order to
			       get an initial value for the FPU
			       context...
			     */

	/* First of all, init the exc and irq tables... */
	for(i = 0; i < 16; i++) {
		ll_irq_table[i] = (DWORD)dummyfun;
		ll_exc_table[i] = (DWORD)dummyfun;
	}

	X86_get_CPU(&cpuInfo);
	X86_get_FPU();
	ll_arch.x86.arch = __LL_ARCH__;
	ll_arch.x86.cpu = cpuInfo.X86_cpu;
	ll_arch.x86.fpu = X86_fpu;
	memcpy(&(ll_arch.x86.vendor), &(cpuInfo.X86_vendor_1), 12);

	/* TODO! Need to map featuresXXX & Signature onto ll_arch!  */
	/* TODO! Need to check for CPU bugs!!           */
	
	#ifdef __LL_DEBUG__
	message("LL Architecture: %s\n", __LL_ARCH__);
	message("CPU : %u\nFPU : %u\n", cpuInfo.X86_cpu, X86_fpu);
	message("Signature : 0x%lx\nVendor: %s\n", cpuInfo.X86_signature,
						ll_arch.x86.vendor);
	message("Features #1: 0x%lx\n", cpuInfo.X86_IntelFeature_1);
	message("Features #2: 0x%lx\n", cpuInfo.X86_IntelFeature_2);
	message("Features #3: 0x%lx\n", cpuInfo.X86_StandardFeature);
	#endif /* __LL_DEBUG__ */

	/* Insert the Exceptions handler into IDT */
	IDT_place(0x00,exc0);
	IDT_place(0x01,exc1);
	IDT_place(0x02,exc2);
	IDT_place(0x03,exc3);
	IDT_place(0x04,exc4);
	IDT_place(0x05,exc5);
	IDT_place(0x06,exc6);
	IDT_place(0x07,exc7);
	IDT_place(0x08,exc8);
	IDT_place(0x09,exc9);
	IDT_place(0x0A,exc10);
	IDT_place(0x0B,exc11);
	IDT_place(0x0C,exc12);
	IDT_place(0x0D,exc13);
	IDT_place(0x0E,exc14);
	IDT_place(0x0F,exc15);
	IDT_place(0x10,exc16);
	/* Insert HW timer handler into IDT */
/* Now it is done in event.c
	IDT_place(0x40,ll_timer);
*/
	IDT_place(0x41,h1);
	IDT_place(0x42,h2);
	IDT_place(0x43,h3);
	IDT_place(0x44,h4);
	IDT_place(0x45,h5);
	IDT_place(0x46,h6);
	IDT_place(0x47,h7);
	IDT_place(0x70,h8);
	IDT_place(0x71,h9);
	IDT_place(0x72,h10);
	IDT_place(0x73,h11);
	IDT_place(0x74,h12);
	
	/* If FPU is on-chip IRQ #13 is free to use */    
	/* Else IRQ #13 vectors the coprocessor errors */
	if (check_fpu()) ll_arch.x86.capabilities |= LL_X86_INTERNAL_FPU;

	#ifdef __LL_DEBUG__
	message("Check FPU : %s\n",ll_arch.x86.capabilities & LL_X86_INTERNAL_FPU ? "Internal" : "External");
	#endif

	if (ll_arch.x86.capabilities & LL_X86_INTERNAL_FPU) {
	/* Install the generic handler */
	IDT_place(0x75,h13);
	} else {
	/* Install the external FPU error handler */
	IDT_place(0x75,h13_bis);
	irq_unmask(13);
	}
	IDT_place(0x76,h14);
	IDT_place(0x77,h15);

	/* Init TSS table & put the corrispondent selectors into GDT */
	TSS_control[TSSMain] |= TSS_USED;
	for (i = 0; i < TSSMax; i++) {
	/* b = appl2linear(&TSS_table[i]); */
	b = (LIN_ADDR)(&TSS_table[i]);
	GDT_place(TSSindex2sel(i),(DWORD)b,sizeof(TSS),FREE_TSS386, GRAN_16);
	}
	/* Set the TR initial value */
	init_TR(TSSindex2sel(TSSMain));
	
	/* Init coprocessor & assign it to main() */
	/* OK... Now I know the sense of all this... :
		We need a initial value for the FPU context (to be used for creating
	new FPU contexts, as init value)...
	... And we get it in this strange way!!!!
	*/
	reset_fpu();
#if 0
	ll_FPU_save();
	memcpy(ll_FPU_stdctx,ll_FPU_savearea,FPU_CONTEXT_SIZE);
#else
	save_fpu(&dummy_tss); /* OK???*/
	memcpy(ll_FPU_stdctx, dummy_tss.ctx_FPU, FPU_CONTEXT_SIZE);
#endif
	init_fpu();

	/* Init PIC controllers & unmask timer */
	PIC_init();


	return mbi_address();
}


void x_end(void)
{
	outp(0x21,0xFF);
	outp(0xA1,0xFF);
	/* Back to DOS settings */
	PIC_end();
	/* Reset the timer chip according DOS specification */
	/* Mode: Binary/Mode 3/16 bit Time_const/Counter 0 */
#if 0
	outp(0x43,0x36);
	/* Time_const = 65536; write 0 in CTR */
	outp(0x40,0);
	outp(0x40,0);
#endif
	pit_init(0, TMR_MD3, 0);    /* Timer 0, Mode 3, Time constant 0 */
	if(ll_arch.x86.cpu > 4) {	
		pit_init(1, TMR_MD2, 18);
	} else {
		pit_init(2, TMR_MD0, 0);
		outp(0x61, 0);              /* Stop channel 2 */
	}
}
