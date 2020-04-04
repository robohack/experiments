#
# fun with GNU Make pattern rules and "override"
#
# assuming sources exist, running "gmake foo_test" should make it
#

LIB = test

override CFLAGS += -W -Wall -Werror -Wextra

SRCS = a.c b.c
OBJS = $(SRCS:%.c=%.o)

# a special source containing extra test helpers not in the LIB
TESTSRCS = unit_test.o
TESTOBJS = $(TESTSRCS:%.c=%.o)

HDR = libtest.h

all: lib$(LIB).a $(TESTOBJS)

lib$(LIB).a: $(OBJS)
	$(AR) $(ARFLAGS) $@ $^
	ranlib $(@)

$(OBJS): $(HDR)

# note this isn't strictly necessary for the test programs alone as they depend
# on, and link with, the library filename directly (i.e. not using '-ltest')
#
override LDFLAGS += -L.

# a pattern rule to make test programs for the library where their (single file)
# sources might be named arbitrarily like:  *_test.c
#
# XXX for some reason a pattern rule can't have more than two dependencies
# unless the additional ones already exist or are already built by other rules,
# thus they (like unit_test.o) must be pre-built by some other rule, such as
# "all"
#
#%: %.c $(TESTOBJS) lib$(LIB).a
#	$(LINK.c) -o $@ $^
#
%_test: %_test.c $(TESTOBJS) lib$(LIB).a
	$(LINK.c) -o $@ $^

#
# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (let ((fn (file-name-nondirectory (buffer-file-name)))) (concat "touch a.c b.c t_test.c libtest.h unit_test.c; gmake -f " fn " CFLAGS=-O0 t_test; rm a.[co] b.[co] t_test.c libtest.h libtest.a unit_test.o t_test"))
# End:
#
