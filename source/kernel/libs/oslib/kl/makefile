# Standard library for X/COFF kernel
# Makefile for GNU MAKE & GCC 2.8.0

#
# Standard path
#

ifndef BASE
	BASE = ..
	BASEDOS = ..
endif

include $(BASE)/config.mk

C_OPT += -D__VIRCSW__ -D__NOH4__
ASM_OPT += -D__VIRCSW__

C_OPT += -DPROFILE
ASM_OPT += -DPROFILE
KL_C_OBJ = stuff.o \
	mem.o \
	cxsw-2.o \
	init.o \
	time.o \
	event.o \
	event1.o \
	aspace.o

KL_OBJ = $(KL_C_OBJ) cxsw-1.o

#
# Ok! Finally the dependency rules!
# We do not mess with automatic depencencies here!!
#

.PHONY : all clean info install

info :
	@echo "OSLib Makefile"
	@echo "Chose: all, install, clean"
	

all : libkl.a

install : libkl.a $(LIB_DIR)
	$(CP) libkl.a $(LIB_DIR)

$(LIB_DIR) :
	$(MKDIR) $(LIB_DIR)
clean :
	$(RM) *.o
	$(RM) *.err
	$(RM) libkl.a

allclean : clean
	echo # Kernel Dependency file > deps
	$(RM) ..\lib\libkl.a

deps :$(KL_C_OBJ:.o=.c)
	$(CC) $(C_OPT) $(KLINCL) -M $(KL_C_OBJ:.o=.c) > deps

libkl.a : $(KL_OBJ)
	$(AR) rs libkl.a $(KL_OBJ)

etest: estub.o event.o
	gcc estub.o event.o -o etest

ifeq (deps,$(wildcard deps))
include deps
endif
