/* @(#)w_acosh.c 5.1 93/09/24 */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 */

#ifndef lint
static char rcsid[] = "$\Id: w_acosh.c,v 1.2 1995/05/30 05:50:39 rgrimes Exp $";
#endif

/*
 * wrapper acosh(x)
 */

#include "math.h"
#include "math_private.h"

#ifdef __STDC__
	double acosh(double x)		/* wrapper acosh */
#else
	double acosh(x)			/* wrapper acosh */
	double x;
#endif
{
#ifdef _IEEE_LIBM
	return __ieee754_acosh(x);
#else
	double z;
	z = __ieee754_acosh(x);
	if(_LIB_VERSION == _IEEE_ || isnan(x)) return z;
	if(x<1.0) {
	        return __kernel_standard(x,x,29); /* acosh(x<1) */
	} else
	    return z;
#endif
}
