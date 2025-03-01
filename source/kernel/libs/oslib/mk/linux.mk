CC  = gcc
AS  = gcc
LD  = ld
AR  = ar
INCL   = $(BASE)
VMINCL  = -I$(BASE)/include/i386 -I$(BASE)/include -I$(BASE)/include/sys/ll
LIB_PATH    = $(BASE)/lib/
LIB_DIR  = $(BASE)/lib

C_OPT =  -Wall -O -finline-functions -fno-builtin -nostdinc -D__LINUX__ -I$(INCL)
ASM_OPT =  -x assembler-with-cpp -D__LINUX__ -I$(INCL)
LINK_OPT = -Bstatic -Ttext 0x220000 -s -nostartfiles -nostdlib -L$(LIB_PATH)

MKDIR   = mkdir
CP	= cp
CAT	= cat
RM	= rm -f

# Common rules

%.o : %.s
	$(REDIR) $(CC) $(ASM_OPT) $(A_OUTPUT) -c $<
%.o : %.c
	$(REDIR) $(CC) $(C_OPT) $(C_OUTPUT) -c $<
%.s : %.c
	$(REDIR) $(CC) $(C_OPT) $(C_OUTPUT) -S $<

