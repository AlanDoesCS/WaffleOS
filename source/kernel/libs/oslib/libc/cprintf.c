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

#include <ll/i386/hw-data.h>
#include <ll/i386/hw-instr.h>
#include <ll/i386/cons.h>
#include <ll/i386/string.h>
#include <ll/i386/stdlib.h>
#include <ll/i386/stdio.h>
#include <ll/stdarg.h>

int cprintf(char *fmt,...)
{
    static char cbuf[500];
    va_list parms;
    int result;
    va_start(parms,fmt);
    result = vsprintf(cbuf,fmt,parms);
    va_end(parms);
    cputs(cbuf);
    return(result);
}

int printf_xy(int x,int y,char attr,char *fmt,...)
{
    char cbuf[200];
    va_list parms;
    int result;

    va_start(parms,fmt);
    result = vsprintf(cbuf,fmt,parms);
    va_end(parms);
    puts_xy(x,y,attr,cbuf);
    return(result);
}
