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

/*	Macros for CONTEXT <-> TSS Translation	*/

#ifndef __LL_I386_TSS_CTX_H__
#define __LL_I386_TSS_CTX_H__

#define TSSMax                  155
#define TSSMain                 (TSSMax-1)
#define TSSBase                 0x100
#define TSSsel2index(sel)       ((sel-TSSBase)/8)
#define TSSindex2sel(i)         (TSSBase + i*8)

#endif
