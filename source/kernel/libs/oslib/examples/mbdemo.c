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

/*	Multiboot Info test file		*/

#include <ll/i386/stdlib.h>
#include <ll/i386/cons.h>
#include <ll/i386/error.h>
#include <ll/i386/mem.h>
#include <ll/i386/mb-info.h>
#include <ll/sys/ll/ll-func.h>
#include <ll/sys/ll/event.h>

int main (int argc, char *argv[])
{
    DWORD sp1, sp2;
    struct ll_initparms parms;
    struct multiboot_info *mbi;
    DWORD lbase, hbase;
    DWORD lsize, hsize;

    sp1 = get_SP();
    cli();
    parms.mode = LL_PERIODIC;
    parms.tick = 1000;
    mbi = ll_init();
    event_init(&parms);

    if (mbi == NULL) {
	    message("Error in LowLevel initialization code...\n");
	    sti();
	    exit(-1);
    }
    sti();

	message("LowLevel started...\n");
	message("MultiBoot informations:\n");
	
	if (mbi->flags & MB_INFO_MEMORY) {
		message("\tMemory informations OK\n");
		lsize = mbi->mem_lower * 1024;
		hsize = mbi->mem_upper * 1024;
		message("Mem Lower: %lx %lu\n", lsize, lsize);
		message("Mem Upper: %lx %lu\n", hsize, hsize);
		if (mbi->flags & MB_INFO_USEGDT) {
			lbase = mbi->mem_lowbase;
			hbase = mbi->mem_upbase;
		} else {
			lbase = 0x0;
			hbase = 0x100000;
		}
		message("\t\tLow Memory: %ld - %ld (%lx - %lx) \n", 
				lbase, lbase + lsize,
				lbase, lbase + lsize);
		message("\t\tLow Memory: %ld - %ld (%lx - %lx)\n", 
				hbase, hbase + hsize,
				hbase, hbase + hsize);
	}
	if (mbi->flags & MB_INFO_BOOTDEV) {
		message("\tBoot device set\n");
	}
	if (mbi->flags & MB_INFO_CMDLINE) {
		message("\tCommand line provided\n");
	}
	if (mbi->flags & MB_INFO_MODS) {
		message("\tSome modules was loaded\n");
	}
	if (mbi->flags & MB_INFO_AOUT_SYMS) {
		message("\tA.OUT Simbol table\n");
	}
	if (mbi->flags & MB_INFO_ELF_SHDR) {
		message("\tELF Section header\n");
	}
	if (mbi->flags & MB_INFO_MEM_MAP) {
		message("\tMemory map provided\n");
	}
	if (mbi->flags & MB_INFO_USEGDT) {
		message("\tLoaded through X\n");
	}
    cli();
    ll_end();
    sp2 = get_SP();
    message("End reached!\n");
    message("Actual stack : %lx - ", sp2);
    message("Begin stack : %lx\n", sp1);
    message("Check if same : %s\n",sp1 == sp2 ? "Ok :-)" : "No :-(");
    
    return 1;
}
