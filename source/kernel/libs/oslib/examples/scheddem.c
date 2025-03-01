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

/*	Stupid Scheduling Demo	*/

#include <ll/i386/cons.h>
#include <ll/i386/tss-ctx.h>
#include <ll/i386/error.h>
#include <ll/sys/ll/ll-instr.h>
#include <ll/sys/ll/ll-func.h>
#include <ll/sys/ll/time.h>
#include <ll/sys/ll/event.h>
#include <ll/stdlib.h>

#define FREE 0
#define IDLE 1

struct ctx {
	WORD ll;
	struct timespec time;
	DWORD cputime;
	DWORD toexec;
	int acc;
	DWORD slice;
	int state;
};

struct trkevt {
	int thread;
	DWORD time;
	int value;
};

struct trkevt track[10000];

DWORD cpu[] = {5000, 2000, 10000, 500, 5000, 6000, 20000, 1000, 7000};
DWORD p[] = {100000, 50000, 100000, 5000, 20000, 30000, 200000, 200000, 35000};

#define STACK_SIZE      100000U           /* Stack size in bytes          */
#define USE_FPU         0x0001

WORD th1, thm;

#define T 1000

#if 1
#define TO     ll_context_to
#define FROM   ll_context_from 
#else
#define TO     ll_context_load
#define FROM   ll_context_save 
#endif

/* For stack allocation checking */
BYTE stacks[STACK_SIZE];
#define N 10

struct ctx context[N];
int exec;



void endProg(void *p)
{
	int i;
	DWORD last[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	DWORD lasttime, lastthread;

	lasttime = 0;
	lastthread = 0;
	for (i = 0; i < 120; i++) {
		if (last[track[i].thread] != 0) {
			if (lastthread == 3) {
				message("Thread %d <-- time %lu (after %lu), Thread %lu executed for %lu --- %d\n",
				track[i].thread,
				track[i].time,
				track[i].time - last[track[i].thread],
				lastthread,
				track[i].time - lasttime, track[i].value);
			}
		}
		lastthread = track[i].thread;
		last[track[i].thread] = track[i].time;
		lasttime = track[i].time;
	}

	ll_abort(110);
}

int schedule(void);


void endBudget(void *p)
{
	struct ctx *c = p;
	struct timespec time, toexec, xcuted, start;
	int newthread;

	ll_gettime(TIME_EXACT, &time);
	NULL_TIMESPEC(&toexec);
	ADDNANO2TIMESPEC(c->toexec, &toexec);
	SUBTIMESPEC(&(c->time), &toexec, &start);
	SUBTIMESPEC(&time, &start, &xcuted);
	NULL_TIMESPEC(&toexec);
	ADDNANO2TIMESPEC(c->cputime * 1000, &toexec);
	SUBTIMESPEC(&xcuted, &toexec, &start);
	c->acc += TIMESPEC2NANOSEC(&start);

	newthread = schedule();
	TO(context[newthread].ll);
}

int schedule(void)
{
	static int k = 0;
	int toe;
	int done;

	done = 0;
	while(!done) {
		exec = (exec + 1) % N;
		while(context[exec].state == FREE) {
			exec = (exec + 1) % N;
		}
/*
		track[k].thread = exec;
		track[k].value = context[exec].acc;
		track[k].time = ll_gettime(TIME_EXACT, &(context[exec].time));
		k++;
*/
		ll_gettime(TIME_EXACT, &(context[exec].time));
		toe = (context[exec].cputime * 1000 - context[exec].acc);
		if (toe <= 0) {
			context[exec].acc -= (context[exec].cputime * 1000);
			k--;
		} else {
			done = 1;
		}
	}
	context[exec].toexec = toe;
	ADDNANO2TIMESPEC(toe, &(context[exec].time));
	event_post(context[exec].time, endBudget, &(context[exec]));
	return exec;
}





void thread(void *p)
{
	int i, ii;
	int position = 0;
	char mark[11];
	int active = 1;
	WORD count = 0;

	i = *(int *)p;
	while(active) {
/*
		printf_xy(10, i * 2 + 1, WHITE, "%c", mark);
*/
		count++;
		position = (position + 1) % 10;
		for (ii = 0; ii < 10; ii++) {
			mark[ii] = ' ';
		}
		mark[10] = 0;
		mark[position] = 'O';
		printf_xy(10, i * 2 + 1, WHITE, "%s", mark);
		printf_xy(25, i * 2 + 1, WHITE, "%d %u", i, count);
		for (ii = 0; ii < 10000; ii++);

		if (count == 1000) active = 0;
	}
	printf_xy(20, i * 2 + 1, WHITE, "Finished %d", i);
}

void killer(void)
{
	int th;

	cli();
	context[exec].state = FREE;
	th = schedule();
	sti();
	TO(context[th].ll);
}

int main(int argc, char *argv[])
{
	DWORD sp1, sp2;
	struct ll_initparms parms;
	void *mbi;
	DWORD t, oldt, secs;
	int th;
	int i;
	int pars[N];
	struct timespec endTime;

	sp1 = get_SP();
	cli();
	parms.mode = LL_PERIODIC;
	parms.tick = T;

	mbi = ll_init();
	event_init(&parms);

	if (mbi == NULL) {
		message("Error in LowLevel initialization code...\n");
		sti();
		exit(-1);
	}
	message("LowLevel started...\n");
	for (i = 0; i < N - 1; i++) {
		pars[i] = i;
		context[i].ll = ll_context_create(thread, &stacks[STACK_SIZE * (i + 1) / N ], &(pars[i]), killer, 0);
		if (context[i].ll == 0) {
			error("Unable to create thread");
			ll_abort(100);
		}
		context[i].state = IDLE;
		context[i].cputime = cpu[i];
		context[i].toexec = cpu[i];
		context[i].acc = 0;
		context[i].slice = p[i];
	}

	context[N - 1].ll = FROM();
	context[N - 1].state = IDLE;
	context[N - 1].cputime = 200;
	context[N - 1].slice = 20000;

message("All Threads created...\n");
	NULL_TIMESPEC(&endTime);
	endTime.tv_sec = 30;

	event_post(endTime, endProg, NULL);
	sti();

	cli();
	th = schedule();
	sti();

	TO(context[th].ll);

message("Thread MAIN restartin'\n");

	secs = 0; oldt = 0;
	while (secs <= 20) {
		cli();
		t = ll_gettime(TIME_EXACT, NULL);
		sti();
		if (t < oldt) {
			message("ARGGGGG! %lu %lu\n", t, oldt);
			ll_abort(99);
		}
		oldt = t;
		if ((t  / 1000000) > secs) {
			secs++;
			printf_xy(40, 20, WHITE, "%lu     %lu    ", secs, t);
		}
	}
	message("\n DONE: Secs = %lu\n", secs);

	cli();
	ll_end();
	sp2 = get_SP();
	message("End reached!\n");
	message("Actual stack : %lx - ", sp2);
	message("Begin stack : %lx\n", sp1);
	message("Check if same : %s\n",sp1 == sp2 ? "Ok :-)" : "No :-(");
	return 1;
}
