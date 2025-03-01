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

/*	Only to test if the VM include is OK... */

#include <ll/ll.h>

#define T  1000

int main (int argc, char *argv[])
{
    DWORD sp1, sp2;
    struct ll_initparms parms;
    struct multiboot_info *mbi;

    sp1 = get_SP();
    parms.mode = LL_PERIODIC;
    parms.tick = T;
    mbi = ll_init();
    event_init(&parms);

    if (mbi == NULL) {
	    message("Error in LowLevel initialization code...\n");
	    exit(-1);
    }
    
    ll_end();
    sp2 = get_SP();
    message("End reached!\n");
    message("Actual stack : %lx - ", sp2);
    message("Begin stack : %lx\n", sp1);
    message("Check if same : %s\n",sp1 == sp2 ? "Ok :-)" : "No :-(");
    
    return 1;
}
