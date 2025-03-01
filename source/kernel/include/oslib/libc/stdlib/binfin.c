#include <malloc.h>
#include <stdio.h>
#include "..\..\h\x86\mb-hdr.h"

//	Ciao :)
void main(int argc, char *argv[])
{
    FILE *in;
    char *buf,*p;
    register scan;
    unsigned long multiboot_sign = MULTIBOOT_MAGIC, size;
    unsigned long *search;
    
    if (argc != 2) {
	printf("Usage: binfin <file>\n");
	exit(0);
    }
    in = fopen(argv[1],"rb");
    if (in == NULL) {
	printf("Error! File %s not found\n",argv[1]);
	exit(-1);
    }
    size = filelength(fileno(in));
    buf = malloc(size);
    if (buf == NULL) {
	printf("Malloc error! Requested : %lu\n",size);
	exit(-2);
    }
    fread(buf,1,size,in);
    fclose(in);
    for (scan = 0, p = buf; scan < size; scan++,p++) {
	search = (unsigned long *)(p);
	if (*search == multiboot_sign) {
	    printf("Gotta! Offset = %lx(%lu)\nAligned = %s",scan,scan, scan == (scan & ~0x00000003) ? "Yes" : "No" );
	    free(buf);
	    exit(1);	    
	}
    }
    printf("Not found!\n");
    exit(1);
}
