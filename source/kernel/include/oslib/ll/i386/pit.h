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

/*	The Programmable Interrupt Timer management code	*/

#ifndef __PIT_H__
#define __PIT_H__

#include <ll/i386/defs.h>
BEGIN_DEF

#include <ll/i386/hw-data.h>
#include <ll/i386/hw-instr.h>

#define        TMR_CTRL        0x43        /* PIT Control port*/
#define        TMR_CNT0        0x40        /* Counter 0 port */
#define        TMR_CNT1        0x41        /* Counter 1 port */
#define        TMR_CNT2        0x42        /* Counter 2 port */

#define        TMR_SC0        0x00        /* Select Channel 0 */
#define        TMR_SC1        0x40        /* Select Channel 1 */
#define        TMR_SC2        0x80        /* Select Channel 2 */

#define        TMR_LSB        0x10        /* R/W Least Significative Byte  */
#define        TMR_MSB        0x20        /* R/W Most Significative Byte   */
#define        TMR_BOTH       0x30        /* R/W Both Bytes                */
#define        TMR_LATCH        0x00      /* Latch Command */
#define        TMR_READ         0xF0      /* Read Command  */
#define        TMR_CNT          0x20      /* Read Counter  */
#define        TMR_STAT         0x10      /* Read Status   */
#define        TMR_CH2          0x08      /* Read Channel 2 Counter/Status */
#define        TMR_CH1          0x04      /* Read Channel 1 Counter/Status */
#define        TMR_CH0          0x02      /* Read Channel 0 Counter/Status */


#define        TMR_MD0        0x00        /* Mode 0 */
#define        TMR_MD1        0x02        /* Mode 1 */
#define        TMR_MD2        0x04        /* Mode 2 */
#define        TMR_MD3        0x06        /* Mode 3 */
#define        TMR_MD4        0x08        /* Mode 4 */
#define        TMR_MD5        0x0A        /* Mode 5 */


INLINE_OP int pit_init(BYTE channel, BYTE mode, WORD tconst)
{
	BYTE v, ch;
	WORD cnt;

	switch (channel) {
		case 0:
			cnt = TMR_CNT0;
			ch = TMR_SC0;
			break;
		case 1:
			cnt = TMR_CNT1;
			ch = TMR_SC1;
			break;
		case 2:
			cnt = TMR_CNT2;
			ch = TMR_SC2;
			break;
		default:
			return -1;
	}

	/* VM_out(TMR_CTRL, 0x34); */
	outp(TMR_CTRL, ch | TMR_BOTH | mode);
	/* Load Time_const with 2 access to CTR */
	v = (BYTE)(tconst);
	outp(cnt, v);
	v = (BYTE)(tconst >> 8);
	outp(cnt, v);

	return 1;
}

INLINE_OP int pit_setconstant(BYTE channel, DWORD c)
{
	BYTE v;
	WORD cnt;
	WORD tconst;

	if (c > 0xF000) {
	  tconst = 0xF000;
	} else {
	  if (c < 10) {
	    tconst = 10;
	  } else {
	    tconst = c;
	  }
	}

	switch (channel) {
		case 0:
			cnt = TMR_CNT0;
			break;
		case 1:
			cnt = TMR_CNT1;
			break;
		case 2:
			cnt = TMR_CNT2;
			break;
		default:
			return -1;
	}

	/* Load Time_const with 2 access to CTR */
	v = (BYTE)(tconst);
	outp(cnt, v);
	v = (BYTE)(tconst >> 8);
	outp(cnt, v);

	return 1;
}


INLINE_OP WORD pit_read(BYTE channel)
{
    WORD result;
    WORD cnt;
    BYTE ch;
    BYTE str_msb, str_lsb;

    switch (channel) {
      case 0:
	cnt = TMR_CNT0;
	ch = TMR_CH0;
	break;
      case 1:
	cnt = TMR_CNT1;
	ch = TMR_CH1;
	break;
      case 2:
	cnt = TMR_CNT2;
	ch = TMR_CH2;
	break;
      default:
	return 0;
    }
    
    /* Read Back Command on counter 0 */
#if 0
outp(TMR_CTRL, ch | TMR_LATCH | TMR_BOTH);
#else
    outp(TMR_CTRL, TMR_READ - TMR_CNT + ch /*0xD2*/);
#endif
    /* Read the latched value from STR */
    str_lsb = inp(cnt);
    str_msb = inp(cnt);
    /* Combine the byte values to obtain a word */
    result = ((WORD)str_msb << 8) | (WORD)str_lsb;
    return result;
}

struct pitspec {
	long	units;
	long    gigas;
};

#define ADDPITSPEC(n, t)  ((t)->units += (n), \
		(t)->gigas += (t)->units / 1432809, \
		(t)->units %= 1432809)
#define NULLPITSPEC(t)   (t)->units = 0, (t)->gigas = 0
#define PITSPEC2USEC(t) ((((t)->units * 1000) / 1197) \
			 + (((t)->gigas * 1000) * 1197))
#define CPPITSPEC(a, b) (b)->units = (a)->units, (b)->gigas = (a)->gigas
END_DEF
#endif        /* __PIT_H__ */
