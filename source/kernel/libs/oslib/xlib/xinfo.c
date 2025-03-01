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

/* Access the info stucture */

#include <ll/i386/stdlib.h>
#include <ll/i386/hw-func.h>
#include <ll/i386/mb-info.h>
#include <ll/i386/x-bios.h>

FILE(X-Info);

/*
   The x_bios is stored in the low memory area and contains all the
   stuff necessary to perform a BIOS call from PM; it can be accessed
   using a linear pointer, which is returned by the following call!
*/

X_CALLBIOS * x_bios_address(void)
{
    X_CALLBIOS *a = (X_CALLBIOS *)GDT_read(X_CALLBIOS_SEL,NULL,NULL,NULL);
    return (a);
}

struct multiboot_info * mbi_address(void)
{
    /* This is declared in [wc32/gnu]\x0.[asm/s] */
    extern struct multiboot_info *mbi;
    return (mbi);
}

/* Stuff information retrieving function */

WORD X_version(void)
{
    X_CALLBIOS *x_bios = x_bios_address();
    return(x_bios->ver);
}

void X_meminfo(LIN_ADDR *b1,DWORD *s1,LIN_ADDR *b2,DWORD *s2)
{
    struct multiboot_info *mbi = mbi_address();
    if (mbi->flags & MB_INFO_USEGDT) {
	if (b1 != NULL) *b1 = (LIN_ADDR)mbi->mem_upbase;
	if (s1 != NULL) *s1 = mbi->mem_upper * 1024;
	if (b2 != NULL) *b2 = (LIN_ADDR)mbi->mem_lowbase;
	if (s2 != NULL) *s2 = mbi->mem_lower * 1024;
    } else {
	if (b1 != NULL) *b1 = (LIN_ADDR)0x100000;
	if (s1 != NULL) *s1 = mbi->mem_upper * 1024;
	if (b2 != NULL) *b2 = (LIN_ADDR)0x4000;
	if (s2 != NULL) *s2 = mbi->mem_lower * 1024;
    }
/*
#ifdef __OLD_MB__
    if (b1 != NULL) *b1 = 0x100000;
    if (s1 != NULL) *s1 = mbi->mem_upper;
    if (b2 != NULL) *b2 = 0x4000;
    if (s2 != NULL) *s2 = mbi->mem_lower;
#else    
    if (b1 != NULL) *b1 = mbi->mem_upbase;
    if (s1 != NULL) *s1 = mbi->mem_upper;
    if (b2 != NULL) *b2 = mbi->mem_lowbase;
    if (s2 != NULL) *s2 = mbi->mem_lower;
#endif
*/
}

#ifdef CHECK
#include <cons.h>
/* Alert if a wrong addressing is intercepted 	*/
/* Used only for debug purposes			*/

void check_addr(void *addr,char *caller)
{
    extern DWORD _stktop;
    if ((DWORD)(addr) < _stktop) {
	cprintf("CRISIS! Addr : %lx(%lx) Caller was %s\n",(DWORD)(addr),_stktop,caller);
    }
}
#endif
