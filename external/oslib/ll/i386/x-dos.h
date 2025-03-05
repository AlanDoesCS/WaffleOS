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

/* Accessing a DOS filesystem from OSLib/X	*/

#ifndef __LL_I386_X_DOS_H__
#define __LL_I386_X_DOS_H__

#include <ll/i386/defs.h>
BEGIN_DEF

#include <ll/i386/x-bios.h>

typedef struct {
	LIN_ADDR buf;
	LIN_ADDR n1;
	char    n2[80];
	BYTE    mode,index;
	DWORD   handle,offset;
} DOS_FILE;
    
DOS_FILE *DOS_fopen(char *name, char *mode);
void DOS_fclose(DOS_FILE *f);
DWORD DOS_fread(void *buf,DWORD size,DWORD num,DOS_FILE *f);
DWORD DOS_fwrite(void *buf,DWORD size,DWORD num,DOS_FILE *f);
unsigned DOS_error(void);

END_DEF

#endif
