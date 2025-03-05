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

/*	Console output functions	*/

#include <ll/i386/hw-data.h>
#include <ll/i386/hw-instr.h>
#include <ll/i386/cons.h>
/* #include <xsys.h>*/
#include <ll/i386/string.h>
#include <ll/i386/stdlib.h>
#include <ll/i386/stdio.h>
#include <ll/stdarg.h>

/* CGA compatible registers value */

#define CURSOR_START		0x0A
#define CURSOR_END		0x0B
#define VIDEO_ADDRESS_MSB	0x0C
#define VIDEO_ADDRESS_LSB	0x0D
#define CURSOR_POS_MSB		0x0E
#define CURSOR_POS_LSB		0x0F

/* CGA compatible registers */

#define CGA_INDEX_REG	0x3D4
#define CGA_DATA_REG	0x3D5

/* Standard tab size */

#define TAB_SIZE	8

/* Store bios settings */

static unsigned char bios_x,bios_y,bios_start,bios_end,bios_attr;

/* Access directly to video & BIOS memory through linear addressing */

/* Active video page-buffer */
#define PAGE_SIZE		2048
#define PAGE_MAX		8

static int active_page = 0;
static int visual_page = 0;
static int curs_x[PAGE_MAX];
static int curs_y[PAGE_MAX];

void set_visual_page(int page)
{
    unsigned short page_offset = page*PAGE_SIZE;
    visual_page = page;
    outp(CGA_INDEX_REG,VIDEO_ADDRESS_LSB);
    outp(CGA_DATA_REG,page_offset & 0xFF);
    outp(CGA_INDEX_REG,VIDEO_ADDRESS_MSB);
    outp(CGA_DATA_REG,(page_offset >> 8) & 0xFF);
}

void set_active_page(int page)
{
    curs_x[active_page] = bios_x;
    curs_y[active_page] = bios_y;
    bios_x = curs_x[page];
    bios_y = curs_y[page];
    active_page = page;
}

int get_visual_page(void)
{
    return(visual_page);
}

int get_active_page(void)
{
    return(active_page);
}

void place(int x,int y)
{
    unsigned short cursor_word = x + y*80 + active_page*PAGE_SIZE;
    /* Set cursor position				  */
    /* CGA is programmed writing first the Index register */
    /* to specify what internal register we are accessing */
    /* Then we load the Data register with the wanted val */
    outp(CGA_INDEX_REG,CURSOR_POS_LSB);
    outp(CGA_DATA_REG,cursor_word & 0xFF);
    outp(CGA_INDEX_REG,CURSOR_POS_MSB);
    outp(CGA_DATA_REG,(cursor_word >> 8) & 0xFF);
    /* Adjust temporary cursor bios position */
    bios_x = x;
    bios_y = y;
}

void cursor(int start,int end)
{
    /* Same thing as above; Set cursor scan line */
    outp(CGA_INDEX_REG,CURSOR_START);
    outp(CGA_DATA_REG,start);
    outp(CGA_INDEX_REG,CURSOR_END);
    outp(CGA_DATA_REG,end);
}

void bios_save(void)
{
    /* This function must be called to init CONSole output */
#if 1
    bios_attr = lmempeekb((LIN_ADDR)0xB8000 + 159);
    bios_x = lmempeekb((LIN_ADDR)0x00450);
    bios_y = lmempeekb((LIN_ADDR)0x00451);
    bios_end = lmempeekb((LIN_ADDR)0x00460);
    bios_start = lmempeekb((LIN_ADDR)0x00461);
    active_page = visual_page = 0;
#else
    LIN_ADDR p;

    p = (LIN_ADDR)(0xB8000 + 159);
    bios_attr = *p;
    p = (LIN_ADDR)0x00450;
    bios_x = *p;
    p = (LIN_ADDR)0x00451;
    bios_y = *p;
    p = (LIN_ADDR)0x00460;
    bios_end = *p;
    p = (LIN_ADDR)0x00461;
    bios_start = *p;
    active_page = visual_page = 0;
#endif
}

void bios_restore(void)
{
    lmempokeb((LIN_ADDR)0x00450,bios_x);
    lmempokeb((LIN_ADDR)0x00451,bios_y);
    place(bios_x,bios_y);
    cursor(bios_start,bios_end);
}

void _clear(char c,char attr,int x1,int y1,int x2,int y2)
{
    register int i,j;
    WORD w = attr;
    w <<= 8; w |= c;
    for (i = x1; i <= x2; i++)
    	for (j = y1; j <= y2; j++) 
            lmempokew((LIN_ADDR)(0xB8000 + 2*i+160*j + 2*active_page*PAGE_SIZE),w);
    place(x1,y1);
    bios_y = y1;
    bios_x = x1;
}

void clear()
{
    _clear(' ',bios_attr,0,0,79,24);
}

void _scroll(char attr,int x1,int y1,int x2,int y2)
{
    register int x,y;
    WORD xattr = attr << 8,w;
    LIN_ADDR v = (LIN_ADDR)(0xB8000 + active_page*(2*PAGE_SIZE));
      
    for (y = y1+1; y <= y2; y++)
       for (x = x1; x <= x2; x++) {
	   w = lmempeekw((LIN_ADDR)(v + 2*(y*80+x)));
	   lmempokew((LIN_ADDR)(v + 2*((y-1)*80+x)),w);
    }
    for (x = x1; x <= x2; x++)
   	lmempokew((LIN_ADDR)(v + 2*((y-1)*80+x)),xattr);
}

void scroll(void)
{
    _scroll(bios_attr,0,0,79,24);
}

void cputc(char c)
{
    static unsigned short scan_x,x,y;
    LIN_ADDR v = (LIN_ADDR)(0xB8000 + active_page*(2*PAGE_SIZE));
    x = bios_x;
    y = bios_y;
    switch (c) {
	case '\t' : x += 8;		
    		    if (x >= 80) {
			x = 0;
			if (y == 24) scroll();
			else y++;
    		    } else {
			scan_x = 0;
			while ((scan_x+8) < x) scan_x += 8;
			x = scan_x;
		    }
		    break;
	case '\n' : x = 0;
		    if (y == 24) scroll();
		    else y++;
		    break;
	case '\b' : x--;
		    lmempokeb((LIN_ADDR)(v + 2*(x + y*80)),' ');
		    x++;
		    break;
	default   : lmempokeb((LIN_ADDR)(v + 2*(x + y*80)),c);
		    x++;
    		    if (x > 80) {
			x = 0;
			if (y == 24) scroll();
			else y++;
    		    }
    }
    place(x,y);
}

void cputs(char *s)
{
    char c;
    while (*s != '\0') {
	c = *s++;
	cputc(c);
    }
}

void puts_xy(int x,int y,char attr,char *s)
{
    LIN_ADDR v = (LIN_ADDR)(0xB8000 + (80*y+x)*2 + active_page*(2*PAGE_SIZE));
    while (*s != 0) {
	/* REMEMBER! This is a macro! v++ is out to prevent side-effects */
	lmempokeb(v,*s); s++; v++;
	lmempokeb(v,attr); v++;
    }
}

void putc_xy(int x,int y,char attr,char c)
{
    LIN_ADDR v = (LIN_ADDR)(0xB8000 + (80*y+x)*2 + active_page*(2*PAGE_SIZE));
    /* REMEMBER! This is a macro! v++ is out to prevent side-effects */
    lmempokeb(v,c); v++;
    lmempokeb(v,attr);
}

char getc_xy(int x,int y,char *attr,char *c)
{
    LIN_ADDR v = (LIN_ADDR)(0xB8000 + (80*y+x)*2 + active_page*(2*PAGE_SIZE));
    char r;
    r = lmempeekb(v); v++;
    if (c != NULL) *c = r;
    r = lmempeekb(v);
    if (attr != NULL) *attr = r;
    return(r);
}
