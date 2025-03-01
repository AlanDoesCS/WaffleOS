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

/*	VM86 demo: tries to call BIOS services in vm86 mode... */

#include <ll/i386/hw-func.h>
#include <ll/i386/tss-ctx.h>
#include <ll/i386/x-bios.h>
#include <ll/i386/cons.h>
#include <ll/i386/error.h>
#include <ll/stdlib.h>

#define T  1000
#define WAIT()  for (w = 0; w < 0xFFFFFFFF; w++)

static unsigned long int w;

#define __VM86__

#ifdef __VM86__
void vm86BIOSDemo(void)
{
    X_REGS16 ir,or;
    X_SREGS16 sr;
    /* Print ASCII character */
    ir.h.ah = 9;
    /* AL = character to display */
    ir.h.al = '+';
    /* BH = page number, BL = attribute */
    ir.x.bx = 3;
    /* Count number */
    ir.x.cx = 3;
    vm86_callBIOS(0x10,&ir,&or,&sr);
}
#endif

void XBIOSDemo(void)
{
    X_REGS16 ir,or;
    X_SREGS16 sr;
    /* Set video mode */
    ir.h.ah = 9;
    ir.h.al = '+';
    ir.x.bx = 3;
    ir.x.cx = 3;
    X_callBIOS(0x10,&ir,&or,&sr);
}                                                                            

void BIOSDemo(void)
{
    X_REGS16 ir,or;
    X_SREGS16 sr;
    register int i;
    /* Set video mode */
    ir.h.ah = 0;
    ir.h.al = 0x12;
    vm86_callBIOS(0x10,&ir,&or,&sr);
#if 1
    /* Put some pixels */
    for (i = 0; i < 200; i++) {
	ir.h.ah = 0x0C;
	ir.h.al = i % 16;
	ir.x.bx = 0;
	ir.x.dx = i+40;
	ir.x.cx = i+100;
	vm86_callBIOS(0x10,&ir,&or,&sr);
    }
#endif
    WAIT();
    /* Set video mode */
    ir.h.ah = 0;
    ir.h.al = 0x03;    
    vm86_callBIOS(0x10,&ir,&or,&sr);
}

int main (int argc, char *argv[])
{
    DWORD sp1, sp2;
    WORD c;
    int i;
    struct multiboot_info *mbi;

    sp1 = get_SP();
    mbi = x_init();

    if (mbi == NULL) {
	    message("Error in LowLevel initialization code...\n");
	    exit(-1);
    }
    
    message("Starting...");
    c = ll_context_save();
    message("CX=%x\n",c);
    for (i = 0; i < 0x4F000; i++);
#ifdef __VM86__
    vm86_init();	
    BIOSDemo();
#else
    XBIOSDemo();
#endif
    sp2 = get_SP();
    message("End reached!\n");
    message("Actual stack : %lx - ", sp2);
    message("Begin stack : %lx\n", sp1);
    message("Check if same : %s\n",sp1 == sp2 ? "Ok :-)" : "No :-(");
    
    x_end();
    return 1;
}
