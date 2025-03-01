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

/*	Definitions to be used in all .h and .c files	*/

#ifndef __LL_I386_DEFS_H__
#define __LL_I386_DEFS_H__

#ifdef __cplusplus
#define BEGIN_DEF extern "C" {
#else
#define BEGIN_DEF
#endif

#ifdef __cplusplus
#define END_DEF }
#else
#define END_DEF
#endif

#ifdef PROFILE
#define FILE(a) static char FileName[] = "Profile:"#a
#define ASMFILE(a) FileName: .string "Profile:"#a
#else 
#define FILE(a)
#define ASMFILE(a)
#endif
#endif
