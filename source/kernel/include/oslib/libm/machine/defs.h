/*-
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * William Jolitz.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	from: @(#)DEFS.h	5.1 (Berkeley) 4/23/90
 *
 *	$\Id: DEFS.h,v 1.3.2.1 1997/02/14 11:08:46 bde Exp $
 */

/*
 * Just enough to avoid changing too much source code ...
 */

#include <ll/i386/linkage.h>


#ifdef __LINUX__
# define P2ALIGN(p2)	.align	(1<<(p2))
#else
# define P2ALIGN(p2)	.align	p2
#endif
#ifdef __LINUX__
#define FUNCSYM(x)	.type    x,@function
#else
#define FUNCSYM(x)	/* nothing */
#endif

#define TEXT_ALIGN	4

#define	ENTRY(x)	FUNCSYM(x); .globl SYMBOL_NAME(x); P2ALIGN(TEXT_ALIGN); SYMBOL_NAME_LABEL(x)
/*
#define	ENTRY2(x,y)	.globl SYMBOL_NAME(x); .globl SYMBOL_NAME(y); \
			P2ALIGN(TEXT_ALIGN); LEXT(x) LEXT(y)
#define	ASENTRY(x)	.globl x; P2ALIGN(TEXT_ALIGN); gLB(x)
*/

#define CNAME(x)	SYMBOL_NAME(x)

/*
 * No PIC at the moment.
 */
#define PIC_PROLOGUE
#define PIC_EPILOGUE
#define PIC_PLT(x)      x
