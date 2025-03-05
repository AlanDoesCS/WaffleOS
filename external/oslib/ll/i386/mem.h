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

/*	Memory manipulation functions...
	Some of them are derived from Linux	*/

#ifndef __LL_I386_MEM_H__
#define __LL_I386_MEM_H__

#include <ll/i386/defs.h>
BEGIN_DEF

/* Various string manipulation functions */

/* Assembler low level routines 	*/
/* File: Mem.S			        */

#ifndef NULL
    #define NULL 0L
#endif

#include <ll/sys/types.h>
#include <ll/i386/hw-data.h>

/*
#ifndef __HW_DEP_H__
    #include "hw_dep.h"
#endif
*/

extern inline void * __memcpy(void * to, const void * from, size_t n)
{
int d0, d1, d2;
__asm__ __volatile__(
	"cld\n\t"
	"rep ; movsl\n\t"
	"testb $2,%b4\n\t"
	"je 1f\n\t"
	"movsw\n"
	"1:\ttestb $1,%b4\n\t"
	"je 2f\n\t"
	"movsb\n"
	"2:"
	: "=&c" (d0), "=&D" (d1), "=&S" (d2)
	:"0" (n/4), "q" (n),"1" ((long) to),"2" ((long) from)
	: "memory");
return (to);
}

/*
 * This looks horribly ugly, but the compiler can optimize it totally,
 * as the count is constant.
 */
extern inline void * __constant_memcpy(void * to, const void * from, size_t n)
{
	switch (n) {
		case 0:
			return to;
		case 1:
			*(unsigned char *)to = *(const unsigned char *)from;
			return to;
		case 2:
			*(unsigned short *)to = *(const unsigned short *)from;
			return to;
		case 3:
			*(unsigned short *)to = *(const unsigned short *)from;
			*(2+(unsigned char *)to) = *(2+(const unsigned char *)from);
			return to;
		case 4:
			*(unsigned long *)to = *(const unsigned long *)from;
			return to;
		case 6:	/* for Ethernet addresses */
			*(unsigned long *)to = *(const unsigned long *)from;
			*(2+(unsigned short *)to) = *(2+(const unsigned short *)from);
			return to;
		case 8:
			*(unsigned long *)to = *(const unsigned long *)from;
			*(1+(unsigned long *)to) = *(1+(const unsigned long *)from);
			return to;
		case 12:
			*(unsigned long *)to = *(const unsigned long *)from;
			*(1+(unsigned long *)to) = *(1+(const unsigned long *)from);
			*(2+(unsigned long *)to) = *(2+(const unsigned long *)from);
			return to;
		case 16:
			*(unsigned long *)to = *(const unsigned long *)from;
			*(1+(unsigned long *)to) = *(1+(const unsigned long *)from);
			*(2+(unsigned long *)to) = *(2+(const unsigned long *)from);
			*(3+(unsigned long *)to) = *(3+(const unsigned long *)from);
			return to;
		case 20:
			*(unsigned long *)to = *(const unsigned long *)from;
			*(1+(unsigned long *)to) = *(1+(const unsigned long *)from);
			*(2+(unsigned long *)to) = *(2+(const unsigned long *)from);
			*(3+(unsigned long *)to) = *(3+(const unsigned long *)from);
			*(4+(unsigned long *)to) = *(4+(const unsigned long *)from);
			return to;
	}
#define COMMON(x) \
__asm__ __volatile__( \
	"cld\n\t" \
	"rep ; movsl" \
	x \
	: "=&c" (d0), "=&D" (d1), "=&S" (d2) \
	: "0" (n/4),"1" ((long) to),"2" ((long) from) \
	: "memory");
{
	int d0, d1, d2;
	switch (n % 4) {
		case 0: COMMON(""); return to;
		case 1: COMMON("\n\tmovsb"); return to;
		case 2: COMMON("\n\tmovsw"); return to;
		default: COMMON("\n\tmovsw\n\tmovsb"); return to;
	}
}
  
#undef COMMON
}

#define __HAVE_ARCH_MEMCPY
#define memcpy(t, f, n) \
(__builtin_constant_p(n) ? \
 __constant_memcpy((t),(f),(n)) : \
 __memcpy((t),(f),(n)))

extern inline void *lmemcpy(LIN_ADDR t, LIN_ADDR f, size_t n)
{
	void *p1;
	void *p2;

	p1 = (void *)(t);
	p2 = (void *)(f);
	return memcpy(p1, p2, n);
}

#define __HAVE_ARCH_MEMMOVE
extern inline void * memmove(void * dest,const void * src, size_t n)
{
int d0, d1, d2;
if (dest<src)
__asm__ __volatile__(
	"cld\n\t"
	"rep\n\t"
	"movsb"
	: "=&c" (d0), "=&S" (d1), "=&D" (d2)
	:"0" (n),"1" (src),"2" (dest)
	: "memory");
else
__asm__ __volatile__(
	"std\n\t"
	"rep\n\t"
	"movsb\n\t"
	"cld"
	: "=&c" (d0), "=&S" (d1), "=&D" (d2)
	:"0" (n),
	 "1" (n-1+(const char *)src),
	 "2" (n-1+(char *)dest)
	:"memory");
return dest;
}

#define memcmp __builtin_memcmp

#define __HAVE_ARCH_MEMCHR
extern inline void * memchr(const void * cs,int c,size_t count)
{
int d0;
register void * __res;
if (!count)
	return NULL;
__asm__ __volatile__(
	"cld\n\t"
	"repne\n\t"
	"scasb\n\t"
	"je 1f\n\t"
	"movl $1,%0\n"
	"1:\tdecl %0"
	:"=D" (__res), "=&c" (d0) : "a" (c),"0" (cs),"1" (count));
return __res;
}

extern inline void * __memset_generic(void * s, char c,size_t count)
{
int d0, d1;
__asm__ __volatile__(
	"cld\n\t"
	"rep\n\t"
	"stosb"
	: "=&c" (d0), "=&D" (d1)
	:"a" (c),"1" (s),"0" (count)
	:"memory");
return s;
}

/* we might want to write optimized versions of these later */
#define __constant_count_memset(s,c,count) __memset_generic((s),(c),(count))

/*
 * memset(x,0,y) is a reasonably common thing to do, so we want to fill
 * things 32 bits at a time even when we don't know the size of the
 * area at compile-time..
 */
extern inline void * __constant_c_memset(void * s, unsigned long c, size_t count)
{
int d0, d1;
__asm__ __volatile__(
	"cld\n\t"
	"rep ; stosl\n\t"
	"testb $2,%b3\n\t"
	"je 1f\n\t"
	"stosw\n"
	"1:\ttestb $1,%b3\n\t"
	"je 2f\n\t"
	"stosb\n"
	"2:"
	: "=&c" (d0), "=&D" (d1)
	:"a" (c), "q" (count), "0" (count/4), "1" ((long) s)
	:"memory");
return (s);	
}

/*
 * This looks horribly ugly, but the compiler can optimize it totally,
 * as we by now know that both pattern and count is constant..
 */
extern inline void * __constant_c_and_count_memset(void * s, unsigned long pattern, size_t count)
{
	switch (count) {
		case 0:
			return s;
		case 1:
			*(unsigned char *)s = pattern;
			return s;
		case 2:
			*(unsigned short *)s = pattern;
			return s;
		case 3:
			*(unsigned short *)s = pattern;
			*(2+(unsigned char *)s) = pattern;
			return s;
		case 4:
			*(unsigned long *)s = pattern;
			return s;
	}
#define COMMON(x) \
__asm__  __volatile__("cld\n\t" \
	"rep ; stosl" \
	x \
	: "=&c" (d0), "=&D" (d1) \
	: "a" (pattern),"0" (count/4),"1" ((long) s) \
	: "memory")
{
	int d0, d1;
	switch (count % 4) {
		case 0: COMMON(""); return s;
		case 1: COMMON("\n\tstosb"); return s;
		case 2: COMMON("\n\tstosw"); return s;
		default: COMMON("\n\tstosw\n\tstosb"); return s;
	}
}
  
#undef COMMON
}

#define __constant_c_x_memset(s, c, count) \
(__builtin_constant_p(count) ? \
 __constant_c_and_count_memset((s),(c),(count)) : \
 __constant_c_memset((s),(c),(count)))

#define __memset(s, c, count) \
(__builtin_constant_p(count) ? \
 __constant_count_memset((s),(c),(count)) : \
 __memset_generic((s),(c),(count)))

#define __HAVE_ARCH_MEMSET
#define memset(s, c, count) \
(__builtin_constant_p(c) ? \
 __constant_c_x_memset((s),(0x01010101UL*(unsigned char)c),(count)) : \
 __memset((s),(c),(count)))

/*
 * find the first occurrence of byte 'c', or 1 past the area if none
 */
#define __HAVE_ARCH_MEMSCAN
extern inline void * memscan(void * addr, int c, size_t size)
{
	if (!size)
		return addr;
	__asm__("cld\n\t"
			"repnz scasb\n\t"
			"jnz 1f\n\t"
			"dec %%edi\n\t"
			"1:"
			: "=D" (addr), "=c" (size)
			: "0" (addr), "1" (size), "a" (c));
	return addr;
}

void fmemcpy(unsigned short ds,unsigned long dof,unsigned short ss,unsigned long sof,unsigned n);
#if 0
extern inline void fmemcpy(unsigned short ds,unsigned long dof,unsigned short ss,unsigned long sof,unsigned n)
{
	/* Build the standard stack frame */
__asm__ __volatile__(
		/* Get parms into register */
		movl	8(%ebp),%eax
		movw	%ax,%es
		movl	12(%ebp),%edi
		movl	16(%ebp),%eax
		movw	%ax,%ds
		movl	20(%ebp),%esi		
		movl	24(%ebp),%ecx
		cld
		rep 
		"movsb"

	"2:"
	: "=&c" (d0), "=&D" (d1), "=&S" (d2)
	:"0" (n), "q" (n),"1" ((long) to),"2" ((long) from)
	: "memory");

);


		popw	%es
		popw	%ds	
		popl	%edi
		popl	%esi		
		leave
		ret

#endif

END_DEF

#endif
