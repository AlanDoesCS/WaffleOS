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

#include <ll/i386/string.h>

char *strcpy(char *dst,const char *src)
{
    char *retval = dst;
    while (*src != 0) *dst++ = *src++;
    *dst = 0;
    return(retval);
}

char *strncpy(char *dst,const char *src,int n)
{
    char *retval = dst;
    while (*src != 0 && n-- > 0) *dst++ = *src++;
    *dst = 0;
    return(retval);
}

int strcmp(const char *s1,const char *s2)
{
    while (*s1 == *s2) {
	if (*s1 == 0) return 0;
	s1++;
	s2++;
    }
    return *(unsigned const char *)s1 - *(unsigned const char *)(s2);
}

int strncmp(const char *s1,const char *s2,int n)
{
    if (n == 0) return 0;
    do {
	if (*s1 != *s2++)
	    return *(unsigned const char *)s1 - *(unsigned const char *)--s2;
	if (*s1++ == 0) break;
    } while (--n != 0);
    return 0;
}

char *strupr(char *s)
{
    char *base = s;
    while (*s != 0) {
	if (*s >= 'a' && *s <= 'z')
           *s = *s + 'A' -'a';
	s++;
    }
    return(base);
}

char *strlwr(char *s)
{
    char *base = s;
    while (*s != 0) {
	if (*s >= 'A' && *s <= 'Z')
	   *s = *s + 'a' -'A';
	s++;
    }
    return(base);
}

int strlen(const char *s)
{
    register int result = 0;
    while (*s != 0) s++ , result++;
    return(result);
}

char *strcat(char *dst,char *src)
{
    char *base = dst;
    while (*dst != 0) dst++;
    while (*src != 0) *dst++ = *src++;
    *dst = 0;
    return(base);
}

char *strscn(char *s,char *pattern)
{
    char *scan;
    while (*s != 0) {
    	scan = pattern;
    	while (*scan != 0) {
	    if (*s == *scan) return(s);
	    else scan++;
	}
	s++;
    }
    return(NULL);
}

char *strchr(char *s,int c)
{
    while (*s != 0) {
	if (*s == (char)(c)) return(s);
	else s++;
    }
    return(NULL);
}
