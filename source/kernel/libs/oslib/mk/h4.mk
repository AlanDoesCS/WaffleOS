CC  = gcc
AS  = gcc
LD  = ld
AR  = ar
BASE    = ..
XINCL   = -I../../include/vm/x86/new -I../../include/vm -I../../include
VMINCL  = -I../../include/vm/x86/new -I../../include/vm -I../../include/vm/sys/ll -I../../include
LIBS    = $(BASE)/../lib
LIBDIR  = $(BASE)\..\lib

C_OPT =  -O -finline-functions -fno-builtin -D__H4__ -D__GNU__
#C_OPT =  $(INCL) -Wall -O2 -finline-functions -fno-builtin -D__LINUX__
A_OPT =  -x assembler-with-cpp -D__H4__ -D__GNU__
LINK_OPT = -oformat coff-go32 -Bstatic -Ttext 0x540000 -s -nostartfiles -nostdlib -L$(LIBS)

CP  = copy
CAT = @type
RM  = -del
