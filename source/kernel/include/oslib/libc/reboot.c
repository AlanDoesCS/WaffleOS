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

/*	Reset (or halt...) the system	*/

#include <ll/i386/hw-func.h>
#include <ll/i386/error.h>

void halt(void)
{
	message("Halt called");
	__asm__("hlt");
}
/*
 * This code has been taken by the Linux Kernel and it has been
 * freely modified and simplified to do a reboot!
 */

void cold_reboot(void)
{
    reboot(0);
}

void warm_reboot(void)
{
    reboot(0x1234);
}

/*
 * This routine reboots the machine by asking the keyboard
 * controller to pulse the reset-line low. If mode = 0x1234 you
 * tell to BIOS to do a warm boot, else cold boot is performed!
 */
void reboot(int mode)
{
    register int i,j;
    
    lmempokew((LIN_ADDR)0x472,mode);
    for (;;) {
	/* Wait for keyboard controller ready */
	for (i=0; i<0x10000; i++)
	    if ((inp(0x64) & 0x02) == 0) break;
	/* Do nothing for a while... */
	for(j = 0; j < 100000 ; j++);
	/* pulse reset low */
	outp(0xfe,0x64);
	/* Do nothing for a while... */
	for(j = 0; j < 100000 ; j++);
    }
}
