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

/* DOS Interface toward disk		*/

#include <ll/i386/mem.h>
#include <ll/i386/error.h>
#include <ll/string.h>
#include <ll/i386/x-dos.h>
#include <ll/i386/x-dosmem.h>

FILE(X-Dos-File);

/* Basic DOS I/O Flag */

/*#define __DEBUG__*/

#define DOS_READ	0
#define DOS_WRITE	1
#define DOS_RW		2

#define DOS_BUFSIZE	1024	/* I/O auxiliary buffer 	*/
#define DOS_MAXDESCR	16	/* Maximum number of DOS file 	*/

static DOS_FILE DOS_descr[DOS_MAXDESCR];
/* I Hope this array is initialized with 0... */
static BYTE busy[DOS_MAXDESCR];

/* The Last DOS Error occurred! */
static unsigned _DOS_error = 0;

unsigned DOS_error(void)
{
    unsigned v = _DOS_error;
    _DOS_error = 0;
    return(v);
}

DOS_FILE *DOS_fopen(char *name,char *mode)
{
    X_REGS16 ir,or;
    X_SREGS16 sr;
    DOS_FILE *f;
    register int i;

    #ifdef __DEBUG__
        char xname[80];
    #endif

    for (i = 0; busy[i] && i < DOS_MAXDESCR; i++);
    /* Return NULL if no descriptor is available... */
    if (i == DOS_MAXDESCR) return(NULL);
    /* Otherwise, lock the descriptor & remember the index */
    f = &DOS_descr[i]; busy[i] = 1; f->index = i;
    /* Allocate a DOS buffer for the file name */
    f->n1 = DOS_alloc(80);
    strcpy(f->n2,name);
    if (mode[0] == 'r') {
	/* DOS Call: Intr 0x21
		AH = 0x3D  - Open existing file
		AL = 0,1,2 - Read,Write, R/W
	*/
	f->mode = DOS_READ;
	ir.h.ah = 0x3D;
	ir.h.al = f->mode;
    }
    else if (mode[0] == 'w') {
	/* DOS Call: Intr 0x21
		AH = 0x3C  - Create a file
		AL = File attribute [0x20 = Standard,r/w,archive]
	*/
	f->mode = DOS_WRITE;
	ir.h.ah = 0x3C;
	ir.x.cx = 0x20;
    }
    f->offset = 0;
    /* Copy th \0 also! */
    memcpy(f->n1, name, strlen(name) + 1);
    #ifdef __DEBUG__
	memcpy(xname, f->n1, strlen(name) + 1);
	message("Name is : %s -- Mode : %d\n",xname,f->mode);
    #endif

    /* Ask DOS to open File */
    ir.x.dx = DOS_OFF(f->n1);
    sr.ds = DOS_SEG(f->n1);
    X_callBIOS(0x21,&ir,&or,&sr);
    f->handle = (!(or.x.cflag) ? or.x.ax : -1);

    if (f->handle == -1) {
	/* DOS request failed! Release the used resources */
	DOS_free(f->n1,80);
	busy[i] = 0;
	_DOS_error = or.x.ax;
	return(NULL);
    }
        
    /* Allocate the DOS buffer for temporary I/O */
    f->buf = DOS_alloc(DOS_BUFSIZE);
    return(f);
}

void DOS_fclose(DOS_FILE *f)
{
    X_REGS16 ir,or;
    X_SREGS16 sr;

    if (f == NULL || busy[f->index] == 0) return;
    /* DOS Call: Intr 0x21
	    AH = 0x3E  - Close a file
	    BX = File handle
    */
    ir.h.ah = 0x3E;
    ir.x.bx = f->handle;
    X_callBIOS(0x21,&ir,&or,&sr);
    DOS_free(f->buf,DOS_BUFSIZE);
    DOS_free(f->n1,80);
    busy[f->index] = 0;
}

DWORD DOS_fread(void *abuf,DWORD size,DWORD num,DOS_FILE *f)
{
    X_REGS16 ir,or;
    X_SREGS16 sr;
    DWORD count = size*num,now = 0,chunk;
    BYTE done = 0;
    BYTE *buf = (BYTE *)(abuf);
    
    while (done == 0) {
	/* Fragment the read operation ... */
	if (count > DOS_BUFSIZE) chunk = DOS_BUFSIZE;
	else chunk = count;
	/* DOS Call: Intr 0x21
		AH = 0x3F  - Read data using a file handle
		BX = File handle
		CX = Buffer size
		DS:DX = Segment:Offset of the Buffer
	*/
	ir.h.ah = 0x3F;
	ir.x.bx = f->handle;
	ir.x.cx = chunk;
	ir.x.dx = DOS_OFF(f->buf);
	sr.ds = DOS_SEG(f->buf);
	X_callBIOS(0x21,&ir,&or,&sr);
	/* If it was OK ... */
	if (!(or.x.cflag)) {
	    /* Copy data into application buffer */
	    memcpy(buf, f->buf, or.x.ax);
	    buf += or.x.ax;
	    now += or.x.ax;
	    f->offset += or.x.ax;
	    count -= or.x.ax;
	    /*
	        Finish if we have read all the data or
		if we have read less data than how expected
	    */	
	    if (now == size*num || or.x.ax != chunk) done = 1;	    
	} else {
	    done = -1;
	    _DOS_error = or.x.ax;
	}
    }
    return(now);
}

DWORD DOS_fwrite(void *abuf,DWORD size,DWORD num,DOS_FILE *f)
{
    X_REGS16 ir,or;
    X_SREGS16 sr;
    DWORD count = size*num,now = 0,chunk;
    BYTE done = 0;
    BYTE *buf = (BYTE *)(abuf);
    
    while (done == 0) {
	/* Fragment the write operation ... */
	if (count > DOS_BUFSIZE) chunk = DOS_BUFSIZE;
	else chunk = count;
	/* Copy data from application buffer */
	memcpy(f->buf, buf, or.x.ax);
	/* DOS Call: Intr 0x21
		AH = 0x40 - Write data using a file handle
		BX = File handle
		CX = Buffer size
		DS:DX = Segment:Offset of the Buffer
	*/
	ir.h.ah = 0x40;
	ir.x.bx = f->handle;
	ir.x.cx = chunk;
	ir.x.dx = DOS_OFF(f->buf);
	sr.ds = DOS_SEG(f->buf);
	X_callBIOS(0x21,&ir,&or,&sr);
	/* If it was OK ... */
	if (!(or.x.cflag)) {
	    f->offset += or.x.ax;
	    count -= or.x.ax;
	    buf += or.x.ax;
	    now += or.x.ax;
	    if (now == size*num || or.x.ax != chunk) done = 1;	    
	} else {
	    done = -1;
	    _DOS_error = or.x.ax;
	}
    }
    return(now);
}

