#
#	tfail.mk:
#
# Demo two-commands-in-one-script failure
#
# Run with (remove the trailing 'exit' unless running it in emacs!):
#
#	rm -rf tfail; mkdir -p tfail; touch tfail/tfail.trace; make MAKEOBJDIR=tfail -T tfail.trace -f tfail.mk -j 20; rc=$?; sleep 2; echo "Empty files:"; find tfail -size 0 -exec ls -l {} +; fgrep DEATH tfail/*
#
# or:
#
#	rm -rf tfail; mkdir -p tfail; cd tfail; touch tfail.trace; make -T tfail.trace -f ../tfail.mk -j 20; rc=$?; sleep 2; echo "Empty files:"; find tfail -size 0 -exec ls -l {} +; fgrep DEATH *
#
# or:
#
#	rm -rf tfail; mkdir -p tfail; touch tfail/tfail.trace; ENV=/home/more/woods/.shrc time make MAKEOBJDIR=tfail -T tfail.trace -f tfail.mk -j 30; rc=$?; sleep 2; ls -l tfail/*.int; echo "Empty files:"; find tfail -size 0 -exec ls -l {} +; fgrep DEATH tfail/*
#
# When it fails, if it behaves the same as I see in a NetBSD build, then there
# will be one or more .int files, and for each there might also be an empty
# associated .obj file too.
#
# A left-over .int file means the script ended early, and that shouldn't happen
# -- all scripts should run to completion.
#
# An empty .obj file means something interrupted the script and that causes
# errors as it is an incorrectly built, incomplete, product file.
#

# prevent Ksh or NetBSD sh from running any user-controlled setup
#
# This should speed up builds where $ENV is accidentally set to a valid
# pathname.  Even in running this test it appears to shave off about 1/3 of a
# second of user CPU time, and maybe as much as half the system CPU time.  (In
# my own setup with a login shell of Ksh, it is set to a variable expansion that
# fails, but this should still speed up builds by avoiding having to try to
# parse and expand it.)
#
# Note by default on NetBSD the default shell used by make is /bin/sh and by
# default it is passed the option "-q" (which show up in "$-" as "eLqs") because
# of the .echoFlag 'q' option (which, FYI, /bin/ksh doesn't have).  This has the
# effect of hiding what is being read from $ENV, if anything.
#
ENV = 		# empty
.export ENV

# This should, and appears to, mimic how the "DEFSHELL" is set up now for use in
# NetBSD:
#
#.SHELL: name=sh path=/bin/sh hasErrCtl=false \
#	newline="\n" \
#	check="echo \"%s\"\n" \
#	ignore="%s\n" \
#	errout="{ %s \n} || exit $?\n" \
#	echoFlag=q
#	comment="\#"

# This sets up NetBSD sh to be used like a modern shell:
#
# Note the echoFlag=qv!  Without the 'v' it doesn't have the desired effect, yet
# it still doesn't show up in the executed shell's "$-"!
#
# This is slightly more efficient than the default old Bourne sh setup.
#
#.SHELL: name=sh path=/bin/sh hasErrCtl=true \
#	check="set -e" ignore="set +e" \
#	echo="set -v" quiet="set +v" filter="set +v" \
#	echoFlag=qv errFlag=e newline="'\n'" \
#	comment="\#"

# This should set up ksh to be used in the same antiquated way /bin/sh is set up
# to work as the "DEFSHELL" in NetBSD make, i.e. without Echo or Error control
# (as in the original KSH setup in Make, sans using "print" instead of "echo")
#
#.SHELL: name=ksh path=/bin/ksh hasErrCtl=false \
#	newline="\n" \
#	check="print \"%s\"\n" \
#	ignore="%s\n" \
#	errout="{ %s \n} || exit $?\n" \
#	comment="\#"

# This sets up ksh to be used like a modern shell
#
#.SHELL: name=ksh path=/bin/ksh hasErrCtl=true \
#	check="set -e" ignore="set +e" \
#	echo="set -v" quiet="set +v" filter="set +v" \
#	echoFlag=v errFlag=e newline="'\n'" \
#	comment="\#"

# This sets up ksh to be used like a modern shell with tracing support
#
#.SHELL: name=sh path=/bin/ksh hasErrCtl=true \
#	check="set -e" ignore="set +e" \
#	echo="set -v" quiet="set +v" filter="set +v" \
#	echoFlag=v errFlag=e newline="'\n'" \
#	comment="\#"

#.SHELL: name=sh path=/bin/dash hasErrCtl=true \
#	check="set -e" ignore="set +e" \
#	echo="set -v" quiet="set +v" filter="set +v" \
#	echoFlag=v errFlag=e newline="'\n'" \
#	comment="\#"

#.SHELL: name=ksh path=/usr/pkg/bin/dash

# Clear th list first just to eliminate any possible side-effects from
# <sys.mk>...
#
.SUFFIXES:

.SUFFIXES: .src .obj

OBJECT_TARGET	= ${.TARGET}.int

# N.B.:  In this form it is common for ${OBJECT_TARGET} to remain, and sometimes
# for the corresponding ${.TARGET} to also be there, but complete.
#
.src.obj:
#	@trap 'echo INT DEATH! >> ${.TARGET}; exit 2' INT
#	@trap 'echo HUP DEATH! >> ${.TARGET}; exit 11' HUP
#	@trap 'echo PIPE DEATH! >> ${.TARGET}; exit 13' PIPE
#	@trap 'echo TERM DEATH! >> ${.TARGET}; exit 15' TERM
# pretend one compile has a syntax error
	if [ ${.TARGET} = "src-3-23.obj" ]; then exit 1; fi
# simulate compilation
	touch ${OBJECT_TARGET} && echo "compiler: warning about ${.IMPSRC}" && sleep 0.1 && cat ${.IMPSRC} >> ${OBJECT_TARGET}
# simulate ctfconvert, with writing to stdout to trigger SIGPIPE
	touch ${.TARGET} && echo "ctfconvert: warning about ${OBJECT_TARGET}" && sleep 0.3 && cat ${OBJECT_TARGET} >> ${.TARGET} && rm -f ${OBJECT_TARGET}

# This form often reproduces the problem
#
#.src.obj:
## pretend one compile has a syntax error
#	if [ ${.TARGET} = "src-3-9.obj" ]; then  exit 1; fi
##
## job shells hang around and burn CPU for a bit with any of these traps!  (with
## or without the '@' flag) presumably because they try to write to a closed pipe
## but SIGPIPE is caught
##	@trap 'echo INT DEATH!; exit 2' INT
##	@trap 'echo HUP DEATH!; exit 11' HUP
##	@trap 'echo PIPE DEATH!; exit 13' PIPE
##	@trap 'echo TERM DEATH!; exit 15' TERM
##
## AH HA!  They fail if the job (including shell) writes to stdout/stderr!
##
#	@trap 'echo INT DEATH! >> ${.TARGET}; exit 2' INT
#	@trap 'echo HUP DEATH! >> ${.TARGET}; exit 11' HUP
#	@trap 'echo PIPE DEATH! >> ${.TARGET}; exit 13' PIPE
#	@trap 'echo TERM DEATH! >> ${.TARGET}; exit 15' TERM
## simulate compilation
#	touch ${OBJECT_TARGET}
#	sleep 0.1
#	cat ${.IMPSRC} >> ${OBJECT_TARGET}
## simulate ctfconvert
#	touch ${.TARGET}
#	sleep 0.2
#	cat ${OBJECT_TARGET} >> ${.TARGET} && rm -f ${OBJECT_TARGET}


PROD_ITERS ?= 40
SRC_ITERS ?= 40

all: .PHONY info .WAIT

info: .PHONY
	@printf ".SHELL = '${.SHELL}'\n"
	@printf "ENV = '${ENV}'($${ENV})\n"
	@printf "shell params = $${#}:'$${-}'\n"

# magic range expansions from Roland Illig
#
.for _i in ${:U:${:Urange=${PROD_ITERS}}}

all: dir-${_i}

.for _j in ${:U:${:Urange=${SRC_ITERS}}}

SRCS.${_i} += src-${_i}-${_j}.src
OBJS.${_i} += src-${_i}-${_j}.obj

src-${_i}-${_j}.src:
	echo ${.TARGET} > ${.TARGET}
.endfor

#
# pretend each "foo-*" is built in a separate subdirectory so that they can be
# built in parallel
#
dir-${_i}: .PHONY srcs-${_i}
	${MAKE} -f ${MAKEFILE} foo-${_i}

foo-${_i}: ${OBJS.${_i}}
	touch ${.TARGET}
# pretend foo-1 takes a really long time to build
	@if [ ${.TARGET} = "foo-1" ]; then sleep 10; fi
	cat ${OBJS.${_i}} > ${.TARGET}
	@if [ ${.TARGET} = "foo-1" ]; then echo ${.TARGET} finally done!; fi

# make each set of sources separately just to be sure they exist before the
# "dir-N" is built...
#
srcs-${_i}: .PHONY
	${MAKE} -f ${MAKEFILE} do-srcs-${_i}

do-srcs-${_i}: .PHONY ${SRCS.${_i}}

.endfor


#
# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (concat "rm -rf tfail; mkdir -p tfail; touch tfail/tfail.trace; ENV=/home/more/woods/.shrc time make MAKEOBJDIR=tfail -T tfail.trace -f tfail.mk -j 30; rc=$?; sleep 2; ls -l tfail/*.int; echo "Empty files:"; find tfail -size 0 -exec ls -l {} +; fgrep DEATH tfail/*; exit $rc")
# End:
#
