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

char *strstr(const char *haystack, const char *needle)
{
	int hlen;
	int nlen;

	hlen = strlen((char *)haystack);
	nlen = strlen((char *)needle);
	while (hlen >= nlen)
	{
		if (!memcmp(haystack, needle, nlen))
			return (char *)haystack;

		haystack++;
		hlen--;
	}
	return 0;
}
