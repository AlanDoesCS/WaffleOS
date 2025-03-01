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

/*	Time event management functions	*/

#ifndef __LL_SYS_LL_EVENT_H__
#define __LL_SYS_LL_EVENT_H__

#include <ll/i386/defs.h>
BEGIN_DEF

#include <ll/sys/ll/time.h>
#include <ll/sys/ll/ll-data.h>
#define MAX_EVENT 100

struct event {
	struct event *next;		/* Next event in an event queue */
	void *par;			/* Handler's parameter 		*/
	void (*handler)(void *p);	/* Event Handler		*/
	struct timespec time;		/* Time at which the event
					   will raise */
	int index;			/* Event ID 			*/
};

/* Event management functions... */

void event_setprologue(void *p);
void event_setepilogue(void *p);

int (*event_post)(struct timespec time, void (*handler)(void *p), void *par);
int (*event_delete)(int index);

int oneshot_event_post(struct timespec time, void (*handler)(void *p), void *par);
int oneshot_event_delete(int index);
int periodic_event_post(struct timespec time, void (*handler)(void *p), void *par);
int periodic_event_delete(int index);
void event_init(struct ll_initparms *l);

/* Interrupt handler entry */
struct intentry {
	void *par;			/* Handler's parameter		*/
	void (*handler)(void *p);	/* Interrupt Handler		*/
	int index;			/* Interrupt number		*/
	DWORD status;			/* Interrupt status
					   	no handler --> FREE
						handler	--> ASSIGNED
						being served --> BUSY
									*/
	DWORD flags;
};

#define INT_PREEMPTABLE	1
#define INT_FORCE	2

#define INTSTAT_FREE		1
#define INTSTAT_ASSIGNED	2
#define INTSTAT_BUSY		3

int irq_bind(int irq, void (*handler)(void *p), DWORD flags);
int ll_ActiveInt();

END_DEF
#endif
