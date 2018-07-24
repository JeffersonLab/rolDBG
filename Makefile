#
# File:
#    Makefile
#
# Description:
#    Makefile for the rolDBG program(s)
#
#

LINUXVME_LIB	?= ${CODA}/extensions/linuxvme/libs
LINUXVME_INC	?= ${CODA}/extensions/linuxvme/include

CROSS_COMPILE		=
CC			= $(CROSS_COMPILE)gcc
AR                      = ar
RANLIB                  = ranlib
CFLAGS			= -Wall -g \
				-I. -I${LINUXVME_INC} \
				-L. -L${LINUXVME_LIB} \
				-L${CODA_LIB} -lc965_trig_list

PROGS			=

all: $(PROGS)

clean distclean:
	@rm -f $(PROGS) *~ *.so

%: %.c
	echo "Making $@"
	$(CC) $(CFLAGS) -o $@ $(@:%=%.c) -lrt -ljvme -lc965

.PHONY: all clean distclean


#
#  Local Variables:
#  compile-command: "make -k -B rolDBG"
#  End:
#
