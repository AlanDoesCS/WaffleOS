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

/*	CPU detection code	*/

#include <ll/i386/hw-data.h>
#include <ll/i386/hw-arch.h>
#include <ll/i386/mem.h>

FILE(Cpu-C);

INLINE_OP void cpuid(DWORD a, DWORD *outa, DWORD *outb, DWORD *outc, DWORD *outd)
{
#ifdef __OLD_GNU__
    __asm__ __volatile__ (".byte 0x0F,0xA2"
#else
    __asm__ __volatile__ ("cpuid"
#endif
    : "=a" (*outa),
      "=b" (*outb),
      "=c" (*outc),
      "=d" (*outd)
    : "a"  (a));
}

void X86_get_CPU(struct ll_cpuInfo *p)
{
	DWORD tmp;

	memset(p, 0, sizeof(struct ll_cpuInfo));
	if (X86_is386()) {
		p->X86_cpu = 3;
		
		return;
	}
	if (X86_hasCPUID()) {
		p->X86_cpuIdFlag = 1;
		p->X86_cpu = 5;
		cpuid(0, &tmp, &(p->X86_vendor_1), 
				&(p->X86_vendor_3),
				&(p->X86_vendor_2));
		if (tmp >= 1) {
			cpuid(1, &(p->X86_signature),
				&(p->X86_IntelFeature_1),
				&(p->X86_IntelFeature_2),
				&(p->X86_StandardFeature));
		}
	} else {
		p->X86_cpu = 4;
		if (X86_isCyrix()) {
			/* Err... Adjust IT!!!! */
			p->X86_cpu = 11;
		}
		/* Need tests for AMD and others... */
	}
}
