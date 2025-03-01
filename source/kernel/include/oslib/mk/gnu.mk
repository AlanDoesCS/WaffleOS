CC  = gcc
AS  = gcc
LD  = ld
AR  = ar
INCL   = $(BASE)
VMINCL  = -I$(BASE)/include/i386 -I$(BASE)/include -I$(BASE)/include/sys/ll
LIB_PATH    = $(BASE)/lib/
LIB_DIR  = $(BASE)\lib

C_OPT =  -Wall -O -finline-functions -fno-builtin -nostdinc -D__GNU__ -I$(INCL)
ASM_OPT =  -x assembler-with-cpp -D__GNU__ -I$(INCL)
LINK_OPT = -Bstatic -Ttext 0x320000 -oformat coff-go32 -s -nostartfiles -nostdlib -L$(LIB_PATH)

MKDIR	= md
CP	= copy
CAT	= @type
RM	= -del

# Common rules
%.o : %.s
	$(REDIR) $(CC) $(ASM_OPT) $(A_OUTPUT) -c $<
%.o : %.c
	$(REDIR) $(CC) $(C_OPT) $(C_OUTPUT) -c $<
