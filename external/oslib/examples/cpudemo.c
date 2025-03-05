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

/*	CPU Detection test file			*/

#include <ll/i386/stdlib.h>
#include <ll/i386/error.h>
#include <ll/i386/mem.h>
#include <ll/i386/hw-arch.h>
#include <ll/sys/ll/ll-func.h>

#define T  1000

int main (int argc, char *argv[])
{
    DWORD sp1, sp2;
    struct ll_cpuInfo cpu;
    void *res;
    char vendorName[13];

    sp1 = get_SP();
    cli();
    res = ll_init();

    if (res == NULL) {
	    message("Error in LowLevel initialization code...\n");
	    sti();
	    exit(-1);
    }
    sti();

    message("LowLevel started...\n");
    message("CPU informations:\n");
    X86_get_CPU(&cpu);

    message("\tCPU type: %lu\n", cpu.X86_cpu);

    if (cpu.X86_cpuIdFlag) {
    	message("CPUID instruction workin'...\n");
	message("\tCPU Vendor ID: ");

	memcpy(vendorName, &(cpu.X86_vendor_1), 12);
	vendorName[12] = 0;
	message("%s\n", vendorName);

	message("\tCPU Stepping ID: %lx", cpu.X86_cpu & 0x0F);
	message("\tCPU Model: %lx", (cpu.X86_cpu >> 4) & 0x0F);
	message("\tCPU Family: %lx", (cpu.X86_cpu >> 8) & 0x0F);
	message("\tCPU Type: %lx\n", (cpu.X86_cpu >> 12) & 0x0F);

	message("\tStandard Feature Flags %lx\n", cpu.X86_StandardFeature);
	if (cpu.X86_StandardFeature & 0x01) {
		message("Internal FPU present...\n");
	}
	if (cpu.X86_StandardFeature & 0x02) {
		message("V86 Mode present...\n");
	}
	if (cpu.X86_StandardFeature & 0x04) {
		message("Debug Extension present...\n");
	}
	if (cpu.X86_StandardFeature & 0x08) {
		message("4MB pages present...\n");
	}
	if (cpu.X86_StandardFeature & 0x10) {
		message("TimeStamp Counter present...\n");
	}
	if (cpu.X86_StandardFeature & 0x20) {
		message("RDMSR/WRMSR present...\n");
	}
	if (cpu.X86_StandardFeature & 0x40) {
		message("PAE present...\n");
	}
	if (cpu.X86_StandardFeature & 0x80) {
		message("MC Exception present...\n");
	}
	if (cpu.X86_StandardFeature & 0x0100) {
		message("CMPXCHG8B present...\n");
	}
	if (cpu.X86_StandardFeature & 0x0200) {
		message("APIC on chip...\n");
	}
	if (cpu.X86_StandardFeature & 0x1000) {
		message("MTRR present...\n");
	}
	if (cpu.X86_StandardFeature & 0x2000) {
		message("Global Bit present...\n");
	}
	if (cpu.X86_StandardFeature & 0x4000) {
		message("Machine Check present...\n");
	}
	if (cpu.X86_StandardFeature & 0x8000) {
		message("CMOV present...\n");
	}
	if (cpu.X86_StandardFeature & 0x800000) {
		message("MMX present...\n");
	}
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
