/* Project:     OSLib
 * Description: The OS Construction Kit
 * Date:                1.6.2000
 * Idea by:             Luca Abeni & Gerardo Lamastra
 *
 * OSLin is an SO project aimed at developing a common, easy-to-use
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

#ifndef __INTERNAL_SPRINTF_H__
#define __INTERNAL_SPRINTF_H__

/* Flag bit settings */
#define RESPECT_WIDTH	1  /* Fixed width wanted 	*/
#define ADD_PLUS	2  /* Add + for positive/floats */
#define SPACE_PAD	4  /* Padding possibility	*/
#define ZERO_PAD	8
#define LEFT_PAD 	16

#endif
