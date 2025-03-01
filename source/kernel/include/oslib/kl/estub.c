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

#include <stdlib.h>
#include <stdio.h>
#include <hw-data.h>
#include "event.h"

FILE(Event-Stub);

extern struct event *freeevents;
extern struct event *firstevent;
extern TIME actTime;


void called(void)
{
	printf("Called...\n");
}

void event_printqueue(struct event *q)
{
	struct event *p;

	for (p = q; p; p = p->next) {
		printf("Entry %d: Time %d...\n", p->index, p->time);
	}
}

main()
{
	int i, rem;
	event_init();

	printf("Free event queue:\n");
	event_printqueue(freeevents);
	printf("Pending events queue:\n");
	event_printqueue(firstevent);
	
	i = event_post(10, called, NULL);
	printf("Inserted Event %d\n", i);
	
	printf("Free event queue:\n");
	event_printqueue(freeevents);
	printf("Pending events queue:\n");
	event_printqueue(firstevent);
	
	i = event_post(100, called, NULL);
	printf("Inserted Event %d\n", i);

	i = event_post(5, called, NULL);
	printf("Inserted Event %d\n", i);
	i = event_post(50, called, NULL);
	printf("Inserted Event %d\n", i);
	i = event_post(1, called, NULL);
	printf("Inserted Event %d\n", i);
	i = event_post(110, called, NULL);
	printf("Inserted Event %d\n", i);

	printf("Pending events queue:\n");
	event_printqueue(firstevent);

	printf("Now, Wakin' up...\n");

	actTime = 1;
	wake_up(10);
	printf("Pending events queue:\n");
	event_printqueue(firstevent);

	actTime = 70;
	wake_up(10);
	i = event_post(45, called, NULL);
	i = event_post(80, called, NULL);
	i = event_post(20, called, NULL);
	rem = event_post(90, called, NULL);
	i = event_post(105, called, NULL);
	i = event_post(150, called, NULL);
	printf("Pending events queue:\n");
	event_printqueue(firstevent);
	i = event_delete(rem);
	printf("EVT %d removed...OK=%d Pending events queue:\n", rem, i);
	event_printqueue(firstevent);
	i = event_delete(6);
	printf("EVT 6 removed...OK=%d Pending events queue:\n", i);
	i = event_delete(2);
	printf("EVT 2 removed...OK=%d Pending events queue:\n", i);
	i = event_delete(8);
	printf("EVT 8 removed...OK=%d Pending events queue:\n", i);
	event_printqueue(firstevent);
}
