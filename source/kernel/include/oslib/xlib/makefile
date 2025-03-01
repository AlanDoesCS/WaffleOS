# Standard library for X/COFF applications
# Makefile for GNU MAKE & GCC 2.8.0

ifndef BASE
	BASE = ..
	BASEDOS = ..
endif

include $(BASE)/config.mk

C_OPT += -D__VIRCSW__ -D__NOH4__
ASM_OPT += -D__VIRCSW__

C_OPT += -DPROFILE
ASM_OPT += -DPROFILE

COMMON_OBJS = xinfo.o \
	x1.o \
	xsystab.o \
	xbios.o \
	xconv.o \
	xdosf.o \
	xdosm.o \
	ccpu.o \
	fpu.o \
	irq.o \
	ctxsw.o \
	xinit.o \
	vm86.o

GNU_S_OBJS = xsys0.o cpu2.o exc.o ctx.o vm86-exc.o mem.o
#GNU_C_OBJS = gnucomp.o

OBJS = $(GNU_S_OBJS) $(GNU_C_OBJS) $(COMMON_OBJS)

.PHONY : clean allclean info install

info :
	@echo "OSLib Makefile"
	@echo "Chose: all, install, clean"
	
all : libhx.a x0.o

libs : libhx.a

install : libhx.a x0.o $(LIB_DIR)
	$(CP) libhx.a $(LIB_DIR)
	$(CP) x0.o $(LIB_DIR)

$(LIB_DIR) :
	$(MKDIR) $(LIB_DIR)

clean :
	$(RM) *.o
	$(RM) *.err
	$(RM) libhx.a

allclean :
	echo # XTN Library dependencies > deps
	$(RM) $(BASE)\lib\libhx.a
	$(RM) $(BASE)\lib\x0.o

deps: $(COMMON_OBJS:.o=.c) $(patsubst %.o,gnu/%.c,$(GNU_C_OBJS))
	$(CC) $(C_OPT) $(VMINCL) -M $(COMMON_OBJS:.o=.c) $(patsubst %.o,gnu/%.c,$(GNU_C_OBJS)) > deps

#
# The library!!
#
libhx.a : $(OBJS)
	$(AR) rs libhx.a $(OBJS)

ifeq (deps,$(wildcard deps))
include deps
endif
