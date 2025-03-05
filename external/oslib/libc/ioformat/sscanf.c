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

#include <ll/i386/stdlib.h>
#include <ll/i386/string.h>
#include <ll/i386/limits.h>
#include <ll/stdarg.h>
#include <ll/ctype.h>
#include <ll/math.h>
#include "sprintf.h"

#define STD_SIZE	0
#define SHORT_SIZE	1
#define LONG_SIZE	2

int vsscanf(char *buf,char *fmt,va_list parms)
{
    int scanned = 0,size = 0,suppress = 0;
    int w = 0,flag = 0,l = 0;
    char c,*c_ptr;
    long int n1,*n1l;    
    int *n1b;
    short int *n1s;
    long unsigned n2,*n2l,parsing = 0;
    unsigned *n2b;
    short unsigned *n2s;
    double n3,*n3l;
    float *n3s;
    char *base = buf;
    while (*fmt != 0) {
	if (*fmt != '%' && !parsing) {
	    /* No token detected */
	    fmt++;
	}
	else {
	    /* We need to make a conversion */
	    if (*fmt == '%') {
		fmt++;
		parsing = 1;
		size = STD_SIZE;
		suppress = 0;
		w = 0;
		flag = 0;
		l = 0;
	    }
	    /* Parse token */
	    switch(*fmt) {
		case '1' :
		case '2' :
		case '3' :
		case '4' :
		case '5' :
		case '6' :
		case '7' :
		case '8' :
		case '9' :
		case '0' : if (parsing == 1) {
		    	      w = strtou(fmt,10,&base);
			      /* We use SPACE_PAD to parse %10s
			         commands where the number is the
				 maximum number of char to store!
			      */
			      flag |= SPACE_PAD;
			      fmt = base-1;
			   }
			   break;
		case 'c' : c = *buf++;
			   c_ptr = va_arg(parms, char *);
			   *c_ptr = c;
			   scanned++;
			   parsing = 0;
			   break;
		case 's' : c_ptr = va_arg(parms, char *);
			   while (*buf != 0 && isspace(*buf)) buf++;
			   l = 0;
			   while (*buf != 0 && !isspace(*buf)) {
			       if (!(flag & SPACE_PAD)) *c_ptr++ = *buf;
			       else if (l < w) {
				   *c_ptr++ = *buf;
				   l++;
			       }
			       buf++;	     
			   }
			   *c_ptr = 0;
			   scanned++;
			   parsing = 0;
			   break;
		case 'i' :
		case 'd' : buf = strscn(buf,"1234567890-+");
			   n1 = strtoi(buf,10,&base);
			   buf = base;
			   if (!suppress) {
			       switch (size) {
				   case STD_SIZE : n1b = va_arg(parms, int *);
						   *n1b = (int)n1;
						   break;
				   case LONG_SIZE : n1l = va_arg(parms, long int *);
				   		    *n1l = n1;
						    break;
				   case SHORT_SIZE : n1s = va_arg(parms, short int *);
						     *n1s = (short)(n1);
						     break;
			       }
			       scanned++;
			   }
			   parsing = 0;
			   break;
		case 'u' : buf = strscn(buf,"1234567890");
			   n2 = strtou(buf,10,&base);
			   buf = base;
			   if (!suppress) {
			      switch (size) {
				  case STD_SIZE : n2b = va_arg(parms, unsigned *);
				  		  *n2b = (unsigned)n2;
						  break;
				  case LONG_SIZE : n2l = va_arg(parms, long unsigned *);
						   *n2l = n2;
						   break;
				  case SHORT_SIZE : n2s = va_arg(parms, short unsigned *);
						    *n2s = (short)(n2);
						    break;
			      }
			      scanned++;
			   }
			   parsing = 0;
			   break;
		case 'x' : buf = strscn(buf,"1234567890xabcdefABCDEF");
			   n2 = strtou(buf,16,&base);
			   buf = base;
			   if (!suppress) {
			      switch (size) {				  
			          case STD_SIZE : n2b = va_arg(parms, unsigned *);
				  		  *n2b = (unsigned)n2;
						  break;
				  case LONG_SIZE : n2l = va_arg(parms, long unsigned *);
						   *n2l = n2;
						   break;
				  case SHORT_SIZE : n2s = va_arg(parms, short unsigned *);
						    *n2s = (short)(n2);
						    break;
			      }
			      scanned++;
			   }
			   parsing = 0;
			   break;
		case 'f' :
		case 'g' :
		case 'e' : buf = strscn(buf,"1234567890.e+-");
			   n3 = strtod(buf,&base);
			   buf = base;
			   if (!suppress) {			       
			       switch (size) {				   				 
			       	   case STD_SIZE : n3l = va_arg(parms, double *);
						   *n3l = n3;
						   break;
				   case LONG_SIZE : n3l = va_arg(parms, double *);
						    *n3l = n3;
						    break;
				   case SHORT_SIZE : n3s = va_arg(parms, float *);
						     *n3s = (float)(n3);
						     break;
			       }
			       scanned++;
			   }
			   parsing = 0;
			   break;
		case 'l' : size = LONG_SIZE;
			   break;
		case 'h' :
		case 'n' : size = SHORT_SIZE;
			   break;
		case '*' : suppress = 1;
			   break;
		default :  parsing = 0;
			   break;
	    }
	    fmt++;
	}
    }
    return(scanned);
}

int sscanf(char *buf,char *fmt,...)
{
    va_list parms;
    int result;
    va_start(parms,fmt);
    result = vsscanf(buf,fmt,parms);
    va_end(parms);
    return(result);
}
