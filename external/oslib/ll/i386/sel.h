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

/*	Hardware selectors used by OSLib	*/

#ifndef __LL_I386_SEL_H__
#define __LL_I386_SEL_H__

#define NULL_SEL		0x00
#define X_DATA16_SEL		0x08
#define X_CODE16_SEL		0x10
#define X_CODE32_SEL		0x18
#define X_RM_BACK_GATE		0x20
#define X_PM_ENTRY_GATE		0x28
#define X_FLATDATA_SEL		0x30
#define X_FLATCODE_SEL		0x38
#define X_CALLBIOS_SEL		0x40
#define X_CALLBIOS_GATE		0x48

#define X_VM86_TSS		0x50
#define X_MAIN_TSS		0x58
#define X_FLATDATA3_SEL		0x60
#define X_FLATCODE3_SEL		0x68

#endif
