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

/*	Time Event routines	(one shot mode)	*/

#include <ll/i386/stdlib.h>
#include <ll/i386/mem.h>
#include <ll/i386/pit.h>
#include <ll/sys/ll/ll-data.h>
#include <ll/sys/ll/time.h>
#include <ll/sys/ll/event.h>

FILE(EventOneShot);

extern int activeInt;

extern BYTE frc;

extern struct event eventlist[MAX_EVENT];
extern WORD lastTime;
extern struct pitspec globalCounter;

extern struct event *freeevents;
extern struct event *firstevent;

extern void (*evt_prol) (void);
extern void (*evt_epil) (void);

int taratata = 0;
/* TODO: oneshot_event_delete & oneshot_event_init... */

/* Switched to timespec */
int oneshot_event_post(struct timespec time, void (*handler) (void *p), void *par)
{
    struct event *p;
    struct event *p1, *t;
    struct timespec now, tmp;
    int done;
    DWORD tnext;

    if (!freeevents) {
	return -1;
    }
    /* Extract from the ``free events'' queue */
    p = freeevents;
    freeevents = p->next;

    /* Fill the event fields */
    p->handler = handler;
    TIMESPEC_ASSIGN(&(p->time), &time);
    p->par = par;

    /* ...And insert it in the event queue!!! */

    t = NULL;
    done = 0;
    /* walk through list, finding spot, adjusting ticks parameter */
    for (p1 = firstevent; p1; p1 = t->next) {
	if (TIMESPEC_A_GT_B(&time, &p1->time)) {
	    t = p1;
	} else
	    break;
    }
    taratata = 1;

    /* adjust next entry */
    if (t) {
	t->next = p;
    } else {
	firstevent = p;
	ll_gettime(TIME_NEW, &now);
	if (TIMESPEC_A_GT_B(&now, &(firstevent->time))) {
	    NULL_TIMESPEC(&tmp);
	} else {
	    SUBTIMESPEC(&(firstevent->time), &now, &tmp);
	}
	tnext = TIMESPEC2USEC(&tmp);
	tnext = (tnext * 1197) / 1000;
	pit_setconstant(0, tnext);
    }
    p->next = p1;

    return p->index;
}



void oneshot_wake_up(void)
{				/* CHANGE the NAME, please... */
    struct event *p = NULL, *pp;
    struct timespec now, ttmp;
    WORD tmp;
    DWORD tnext;

    tmp = pit_read(frc);
    ADDPITSPEC((WORD) (lastTime - tmp), &globalCounter);
    lastTime = tmp;

    PITSPEC2TIMESPEC(&globalCounter, &now);
    
    if (firstevent != NULL) {
	if (TIMESPEC_A_GT_B(&now, &(firstevent->time))) {
	    if (!activeInt && evt_prol != NULL) {
		evt_prol();
	    }
	    activeInt++;

	    for (p = firstevent; p != NULL; p = pp) {
		if ((p->time.tv_sec > now.tv_sec) ||
		    ((p->time.tv_sec == now.tv_sec) && (p->time.tv_nsec > now.tv_nsec))) {
		    break;
		}
		pp = p->next;
		p->next = freeevents;
		freeevents = p;
		firstevent = pp;
		p->handler(p->par);
	    }

	    activeInt--;
	    if (!activeInt && evt_epil != NULL) {
		evt_epil();
	    }
	}
	tmp = pit_read(frc);
	ADDPITSPEC((WORD) (lastTime - tmp), &globalCounter);
	lastTime = tmp;

	PITSPEC2TIMESPEC(&globalCounter, &now);


	if (TIMESPEC_A_GT_B(&now, &(firstevent->time))) {
	    NULL_TIMESPEC(&ttmp);
	} else {
	    SUBTIMESPEC(&(firstevent->time), &now, &ttmp);
	}
	/*  SUBTIMESPEC(&(firstevent->time), &now, &ttmp); */
	tnext = TIMESPEC2USEC(&ttmp);
	tnext = (tnext * 1197) / 1000;
	pit_setconstant(0, tnext);

    } else {
	pit_setconstant(0, 0xFFFF);
    }
}

int oneshot_event_delete(int index)
{
    struct event *p1, *t;
    struct timespec tmp, now;
    DWORD tnext;

    t = NULL;
    /* walk through list, finding spot, adjusting ticks parameter */

    for (p1 = firstevent; (p1) && (index != p1->index); p1 = t->next) {
	t = p1;
    }

    if (p1 == NULL) {
	return -1;
    }
    if (t == NULL) {
	firstevent = p1->next;
    } else {
	t->next = p1->next;
    }

    p1->next = freeevents;
    freeevents = p1;

    if (firstevent == NULL) {
	pit_setconstant(0, 0xFFFF);
    } else {
	ll_gettime(TIME_NEW, &now);
	if (TIMESPEC_A_GT_B(&now, &(firstevent->time))) {
	    NULL_TIMESPEC(&tmp);
	} else {
	    SUBTIMESPEC(&(firstevent->time), &now, &tmp);
	}
	/*SUBTIMESPEC(&now, &(firstevent->time), &tmp); */
	tnext = TIMESPEC2USEC(&tmp);
	tnext = (tnext * 1197) / 1000;
	pit_setconstant(0, tnext);
    }
    return 1;
}
