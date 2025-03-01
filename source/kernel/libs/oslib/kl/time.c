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

/*	Time management code: ll_getttime()	*/

#include <ll/i386/pit.h>
#include <ll/i386/stdlib.h>
#include <ll/i386/error.h>
#include <ll/i386/mem.h>
#include <ll/sys/ll/ll-data.h>
#include <ll/sys/ll/ll-func.h>
#include <ll/sys/ll/time.h>

/* These are for the EXECT and TICK modes */
extern DWORD ticksize;		/* From init.c	*/
extern struct timespec actTime;	/* From event.c	*/
extern WORD  pit_time_const;	/* From init.c	*/
extern DWORD  timermode;	/* From init.c	*/

/* These two are for the NEW algorithm */
extern WORD lastTime;                /* From event.c */
extern struct pitspec globalCounter; /* From event.c */
extern BYTE frc;


FILE(Time);

TIME ll_gettime(int mode, struct timespec *tsres)
{
	DWORD res, tc;
	BYTE isr;
	struct timespec tmp;
	
	if (mode == TIME_PTICK) {
		if (timermode != LL_PERIODIC) {
			return 0;
		}
		res = TIMESPEC2USEC(&actTime);
		if (tsres != NULL) {
			memcpy(tsres, &actTime, sizeof(struct timespec));
		}
		return res;
	}

	if (mode == TIME_NEW) {
	  WORD tmp;
	  
	  tmp = pit_read(frc);
	  ADDPITSPEC((WORD)(lastTime - tmp), &globalCounter);
	  lastTime = tmp;
	  if (tsres != NULL) {
	    PITSPEC2TIMESPEC(&globalCounter, tsres);
	  }
	  return (PITSPEC2USEC(&globalCounter));
	}

	if (mode == TIME_EXACT) {
		if (timermode == LL_PERIODIC) {
			memcpy(&tmp, &actTime, sizeof(struct timespec));
			/* How much time has elapsed
			 * from the last Tick Boundary?
			 */
			tc = pit_read(0);
			if (tc > pit_time_const) {
			    error("LL Time Panic!!!\n");
			    ll_abort(1);
			}
			res = pit_time_const - tc;
			res *= ticksize;
			res /= pit_time_const;

			/* Detect tick boundary and adjust the time... */
			outp(0x20, 0x0A);
			isr = inp(0x20);
			if ((isr & 1) && res < ((8*ticksize)/10)){
			      /*
				res += ticksize;
				ADDNANO2TIMESPEC(ticksize * 1000, &tmp);
			      */
			      res = ticksize;
			}

			/* Sum the Tick time... */
			ADDNANO2TIMESPEC(res * 1000, &tmp);
			res += TIMESPEC2USEC(&actTime);

			if (tsres != NULL) {
				memcpy(tsres, &tmp, sizeof(struct timespec));
			}
			return res;
		} else {
			return 0;
		}
	}
	return 0;
}
