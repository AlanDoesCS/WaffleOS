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

/*	gettime() test file			*/

#include <ll/i386/stdlib.h>
#include <ll/i386/error.h>
#include <ll/sys/ll/ll-func.h>
#include <ll/sys/ll/event.h>
#include <ll/sys/ll/time.h>

DWORD smain;
DWORD sp;

int sys_abort(int code)
{
    cli();
    ll_abort(code);
    
    /* Never reach this! Just to shut up compiler warning... 	*/
    /* In the general case, a sys_abort can resume...		*/
    return(1);
}

int main (int argc, char *argv[])
{
	DWORD t, t1, oldt, secs;
	struct ll_initparms parms;
	struct timespec ts;
	
	cli();
	message("Init Stack : %lu\n", get_SP());
	
	parms.mode = LL_PERIODIC;
	parms.tick = 1000;
	ll_init();
	event_init(&parms);		/* It's here, but must be changed...*/
	
	smain = get_SP();
	message("Smain --> %lx\n",smain);
	
	sti();
	secs = 0; oldt = 0;
	while (secs <= 120) {
		cli();
		t1 = ll_gettime(TIME_NEW, &ts);
		t = ll_gettime(TIME_EXACT, NULL);
		sti();
		if (t < oldt) {
			error("Time goes back??\n");
			message("ARGGGGG! %lu %lu\n", t, oldt);
			ll_abort(100);
		}
		oldt = t;
		if ((t  / 1000000) > secs) {
			secs++;
			message("%lu                     %lu     %lu/%lu\n", 
					TIMESPEC2USEC(&ts), secs, t, t1);
		}
	}
	message("\n DONE: Secs = %lu\n", secs);
	cli();
	ll_end();
	sp = get_SP();
	message("End reached!\n");
	message("Actual stack : %lx - ",sp);
	message("Main stack : %lx\n",smain);
	message("Check if same : %s\n",smain == sp ? "Ok :-)" : "No :-(");
	return 1;
}
