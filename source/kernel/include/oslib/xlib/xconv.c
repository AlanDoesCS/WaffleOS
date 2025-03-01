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

/* The OSLib (flat space) <-> Phisical address conversion	*/

#include <ll/i386/hw-func.h>
#include <ll/i386/mem.h>

FILE(X-Conv);

/* Conversion between PM address space & phisical address space */
/* If we do not support paging we need to relocate the .ELF executable  */
/* using the segment register; however we also need to access all the   */
/* phisical memory; for example if we allocate some memory (phisical)   */
/* and we use a pointer to access that memory, the pointer will use     */
/* the current DS to address the specified offset; but the phisical     */
/* offset cannot be used because the base of DS is not zero as required */
/* to access phisical memory; the DS base is stored by X into the       */
/* exec_base field of _X_info! This is the key to traslate ELF address  */
/* into phisical address & viceversa					*/

/*
DWORD appl2linear(void *p)
{
    unsigned long result;
    result = (DWORD)(p) + _X_info->exec_base;
    return(result);
}

void *linear2appl(DWORD a)
{
    void *p;
    if (a < _X_info->exec_base) p = NULL;
    else p = (void *)(a-_X_info->exec_base);
    return(p);
}
*/

LIN_ADDR addr2linear(unsigned short seg,unsigned long offset)
{
    LIN_ADDR flatbase;
    flatbase = (LIN_ADDR)GDT_read(seg,NULL,NULL,NULL);
    return(flatbase + offset);
}
