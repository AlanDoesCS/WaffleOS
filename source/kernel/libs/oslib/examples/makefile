# Standard library for X/COFF applications
# Makefile for GNU MAKE & GCC 2.8.0

ifndef BASE
	BASE = ..
	BASEDOS = ..
endif

include $(BASE)/config.mk

all: mbdemo.xtn timetest.xtn eventdem.xtn vmdemo.xtn \
	ctxswdem.xtn scheddem.xtn cpudemo.xtn biosdemo.xtn asdemo.xtn

%.ftp: %.xtn
	ncftpput -u ll -p example thorin . $<
%.flp: %.xtn
	mcopy $< a:

clean :
	$(RM) *.o
	$(RM) *.err
	$(RM) *.xtn

#
# Demo
#

%.xtn : %.o $(LIB_PATH)/libhc.a $(LIB_PATH)/libhm.a $(LIB_PATH)/libhx.a $(LIB_PATH)/libkl.a
	$(LD) $(LINK_OPT) $(LIB_PATH)x0.o $< --start-group -lhc -lhm -lhx -lkl --end-group -o $@
#       $(LD) $(LINK_OPT) $(LIB_PATH)x0.o $< --start-group -lhc -lhx -lkl --end-group -o $@
