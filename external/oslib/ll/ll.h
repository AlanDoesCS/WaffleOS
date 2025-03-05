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

/*	The abominious: an ``include all'' header!!!	*/

#ifndef __LL_LL_H__

#define __LL_LL_H__

#include <ll/sys/ll/aspace.h>
#include <ll/sys/ll/event.h>
#include <ll/sys/ll/exc.h>
#include <ll/sys/ll/ll-data.h>
#include <ll/sys/ll/ll-func.h>
#include <ll/sys/ll/time.h>
#include <ll/sys/ll/ll-instr.h>
#include <ll/sys/ll/ll-mem.h>
#include <ll/i386/cons.h>
#include <ll/i386/error.h>
#include <ll/i386/hw-data.h>
#include <ll/i386/hw-func.h>
#include <ll/i386/hw-instr.h>
#include <ll/i386/hw-io.h>
#include <ll/i386/linkage.h>
#include <ll/i386/mb-hdr.h>
#include <ll/i386/mb-info.h>
#include <ll/i386/mem.h>
#include <ll/i386/sel.h>
#include <ll/i386/tss-ctx.h>
#include <ll/i386/x-bios.h>
#include <ll/i386/x-dos.h>
#include <ll/i386/x-dosmem.h>

#endif
