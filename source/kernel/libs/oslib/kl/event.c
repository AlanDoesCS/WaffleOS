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

/*	Time Event routines	*/

#include <ll/i386/stdlib.h>
#include <ll/i386/mem.h>
#include <ll/i386/error.h>
#include <ll/i386/hw-arch.h>
#include <ll/i386/pit.h>
#include <ll/sys/ll/ll-data.h>
#include <ll/sys/ll/ll-instr.h>
#include <ll/sys/ll/time.h>
#include <ll/sys/ll/event.h>

FILE(Event);

extern LL_ARCH ll_arch;

BYTE frc;

/* Timer 0 usec base tick */
DWORD ticksize;

/* Timer 0 loaded time constant (= ticksize * 1.197) */
WORD pit_time_const;
DWORD timermode;

static DWORD nts;		/* System tick in nanoSeconds... */
struct timespec actTime;	/* Time (in nanosecs)... */
int activeInt = 0;

WORD lastTime;
struct pitspec globalCounter;

struct event eventlist[MAX_EVENT];
struct intentry irqs[16];

struct event *freeevents;
struct event *firstevent;

void (*evt_prol) (void) = NULL;
void (*evt_epil) (void) = NULL;


void event_setprologue(void *p)
{
    evt_prol = p;
}

void event_setepilogue(void *p)
{
    evt_epil = p;
}

int ll_ActiveInt(void)
{
    return activeInt;
}

/* Switched to timespec */
int periodic_event_post(struct timespec time, void (*handler) (void *p),
			void *par)
{
    struct event *p;
    struct event *p1, *t;

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
    /* walk through list, finding spot, adjusting ticks parameter */
    for (p1 = firstevent; p1; p1 = t->next) {
/*
		SUBTIMESPEC(&time, &(p1->time), &tmp);
		if ((tmp.tv_sec > 0) && (tmp.tv_nsec > 0)) {
*/
	if (TIMESPEC_A_GT_B(&time, &p1->time))
	    t = p1;
	else
	    break;
    }

    /* adjust next entry */
    if (t) {
	t->next = p;
    } else {
	firstevent = p;
    }
    p->next = p1;

    return p->index;
}

int periodic_event_delete(int index)
{
    struct event *p1, *t;

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

    return 1;
}

int irq_bind(int irq, void (*handler) (void *p), DWORD flags)
{

    cli();			/* Warning!!! Need to be changed...
				   Protect or not to protect??? 
				   Probably save_flags??? */
    if ((irqs[irq].status != INTSTAT_FREE) &&
	((flags & INT_FORCE) != INT_FORCE)) {
	return -1;
    }

    irqs[irq].status = INTSTAT_ASSIGNED;
    sti();

    if (handler != NULL) {
	irqs[irq].handler = handler;
	irqs[irq].par = &(irqs[irq].index);
	irqs[irq].flags = flags;
    } else {
	irqs[irq].status = INTSTAT_FREE;
    }

    return 1;
}

void periodic_wake_up(void)
{				/* CHANGE the NAME, please... */
    struct event *p, *pp;
    WORD tmp;

    tmp = pit_read(frc);
    ADDPITSPEC((WORD) (lastTime - tmp), &globalCounter);
    lastTime = tmp;

    activeInt++;
    if (activeInt == 1 && evt_prol != NULL) {
	evt_prol();
    }

    ADDNANO2TIMESPEC(nts, &actTime);

    for (p = firstevent; p != NULL; p = pp) {
/*
		SUBTIMESPEC(&(p->time), &actTime, &tmp);
		if ((tmp.tv_sec > 0) && (tmp.tv_nsec > 0)) {
			break;
		} */
	if ((p->time.tv_sec > actTime.tv_sec) ||
	    ((p->time.tv_sec == actTime.tv_sec)
	     && (p->time.tv_nsec > actTime.tv_nsec))) {
	    break;
	}
	pp = p->next;
	p->next = freeevents;
	freeevents = p;
	firstevent = pp;
	p->handler(p->par);
    }

    if (activeInt == 1 && evt_epil != NULL) {
	evt_epil();
    }
    activeInt--;
}

void act_int(BYTE n)
{
    static int ai_called = 0;

    activeInt++;
    if (activeInt == 1 && evt_prol != NULL) {
	evt_prol();
    }
    if (irqs[n].status == INTSTAT_ASSIGNED) {
	irqs[n].status = INTSTAT_BUSY;
	if (irqs[n].flags & INT_PREEMPTABLE) {
	    sti();
	}
	irqs[n].handler(irqs[n].par);
	if (irqs[n].flags & INT_PREEMPTABLE) {
	    cli();
	}
	irqs[n].status = INTSTAT_ASSIGNED;
    }
    ai_called++;
    if (activeInt == 1 && evt_epil != NULL) {
	evt_epil();
    }
    activeInt--;
}

void event_init(struct ll_initparms *l)
{
    extern void ll_timer(void);
    int i;
    BYTE mask;
    TIME t;

    IDT_place(0x40,ll_timer);

    if (l->mode != LL_PERIODIC) {
	error("Trying one-shot!!!");
	t = 0;
	/* Mode: Binary/Mode 4/16 bit Time_const/Counter 0 */
	pit_init(0, TMR_MD4, 0xFFFF);	/* Timer 0, Mode 4, constant 0xFFFF */
    } else {
	t = l->tick;
	/* Translate the tick value in usec into a suitable time constant   */
	/* for 8254 timer chip; the chip is driven with a 1.19718 MHz       */
	/* frequency; then the effective frequency is given by the base     */
	/* frequency divided for the time constant; the tick is the inverse */
	/* of this effective frequency (in usec!)                           */
	/* Time-Constant = f_base (MHz) * tick (usec)                       */
	/* If T-C == 0 -> T-C = 65536 (Max available)                       */
	ticksize = t;
	t = t * 1197;
	t = t / 1000;
	/* Only for security! This should cause timer overrun */
	/* While 0 would set maximum period on timer          */
	if (t == 0)
	    t = 1;
	pit_time_const = (WORD) (t & 0xFFFF);
	/* Mode: Binary/Mode 2/16 bit Time_const/Counter 0 */
	pit_init(0, TMR_MD2, t);	/* Timer 0, Mode 2, Time constant t */


    }
    timermode = l->mode;
   
    if (ll_arch.x86.cpu > 4) { 
	/* Timer1: mode 0, time const 0... */
	pit_init(1, TMR_MD0, 0);
	frc = 1;
    } else {
	frc = 2;
	pit_init(2, TMR_MD0, 0);
	 outp(0x61, 3);
    }

    mask = ll_in(0x21);
    mask &= 0xFE;		/* 0xFE = ~0x01 */
    ll_out(0x21, mask);


    /* Init the event list... */
    for (i = 0; i < MAX_EVENT; i++) {
	if (i < MAX_EVENT - 1) {
	    eventlist[i].next = &(eventlist[i + 1]);
	}
	eventlist[i].index = i;
    }
    eventlist[MAX_EVENT - 1].next = NULL;
    freeevents = &(eventlist[0]);

    /* ...and then the interrupt handlers list!!! */
    for (i = 0; i < 16; i++) {
	irqs[i].status = INTSTAT_FREE;
	irqs[i].index = i;
	irqs[i].handler = NULL;	/* Paranoia */
	irqs[i].par = NULL;	/* Paranoia */
    }
    activeInt = 0;
    evt_prol = NULL;
    evt_epil = NULL;

    /* Initialization of the time variables for periodic mode */
    nts = ticksize * 1000;
    NULL_TIMESPEC(&actTime);

    /* Initialization of the general time variables */
    NULLPITSPEC(&globalCounter);
    lastTime = 0;

    if (timermode == LL_PERIODIC) {
	event_post = periodic_event_post;
	event_delete = periodic_event_delete;
    } else {
	event_post = oneshot_event_post;
	event_delete = oneshot_event_delete;
    }
}
