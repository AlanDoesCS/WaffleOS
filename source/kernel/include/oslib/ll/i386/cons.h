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

/*	Console output functions	*/

#ifndef __LL_I386_CONS_H__
#define __LL_I386_CONS_H__

#include <ll/i386/defs.h>
BEGIN_DEF

/* for reference only */
extern int cons_columns; /* number of screen columns */
extern int cons_rows;    /* number of screen rows */


/* X-dependent Console Output functions			*/
/* Rememeber that console functions are NOT REENTRANT   */
/* The console must be considered a preemtable resource */
/* File : Cons.C					*/

void set_visual_page(int page);
void set_active_page(int page);
int get_visual_page(void);
int get_active_page(void);
void place(int x,int y);
void cursor(int start,int end);
void _clear(char c,char attr,int x1,int y1,int x2,int y2);
void clear(void);
void _scroll(char attr,int x1,int y1,int x2,int y2);
void scroll(void);
void bios_save(void);
void bios_restore(void);
void cputc(char c);
void cputs(char *s);
int  cprintf(char *fmt,...) __attribute__((format(printf,1,2)));

/* These functions allow direct access to video RAM */
/* Hence you can use it without the explicit use of */
/* a resource manager				    */
/* File : Cons.C				    */

void putc_xy(int x,int y,char attr,char c);
char getc_xy(int x,int y,char *attr,char *c);
void puts_xy(int x,int y,char attr,char *s);
int printf_xy(int x,int y,char attr, char *fmt,...) __attribute__((format(printf,4,5)));

/* These are simple useful macro! */
#define HOME()		place(0,0);
#define CRSR_BLOB()	cursor(0,15);
#define CRSR_OFF()	cursor(16,16);
#define CRSR_STD()	cursor(14,15);
#define NL()		cputc('\n');

/* Text mode color definitions */

#define BLACK		0
#define BLUE		1
#define GREEN		2
#define CYAN		3
#define RED		4
#define MAGENTA		5
#define BROWN		6
#define LIGHTGRAY	7
#define DARKGRAY	8
#define LIGHTBLUE	9
#define LIGHTGREEN	10
#define LIGHTCYAN	11
#define LIGHTRED	12
#define LIGHTMAGENTA	13
#define YELLOW 		14
#define WHITE		15

END_DEF

#endif

