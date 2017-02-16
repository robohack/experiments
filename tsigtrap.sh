#! /bin/sh

# note this causes problems with catching a signal when not run from an
# interactive session (e.g. when the 'compile' command is used in emacs...
#
# Also it is not necessary for 'trap CHLD' -- it just makes process exits
# verbose...
#
#set -o monitor

value=${RANDOM:-99999}

echo "value = ${value}...."

# in order to pass values of variables into trap command lists with the values
# at the time of the trap creation one must take care to escape variable
# expansions which are expected to be expanded at trap execution time, as well
# as embedded double quotes:

# one must take care not to set the EXIT (0) trap to '-' if one wants it to also
# fire when the shell exits on the signal the other traps send to it
#
# Note:  However only ksh93 and Bash will execute the EXIT handler when the
# shell sends itself a SIGHUP, SIGINT, (SIGPIPE?,) or SIGTERM signal.

# Note:  Oddly ksh93 won't trigger the CHLD handler if the 'cat' process exits
# normally (i.e. when 'cat' reads EOF and exits) -- apparently it only monitors
# true background processes.

# Note:  Passing on the same signal received to the shell, after clearing the
# trap handler for that signal, as this example does, is probably not always
# what's wanted.  Just use "exit", or maybe pass SIGTERM, instead.

# note: one can also call a function defined in the script from a trap handler
#
EXTRA_TRAP_CODE="echo \"pid \$\$: value = ${value}\";"

# Hmmm... it seems all shells with $RANDOM also have 'trap ERR'
#
if [ ${RANDOM:-0} -ne ${RANDOM:-0} ] ; then
	trap '
		rc=$?;
		if ((ERRNO > 0)); then
			EMSG="; errno: $ERRNO"
		else
			EMSG=""
		fi;
		print "${0#-}: exit code: $rc$EMSG"
	' ERR
fi

# XXX hmmm... setting all traps to be ignored inside the trap handler is OK, but
# only the firing one is sometimes necessary (e.g. with pdksh), and it causes
# weirdness for NetBSD sh if CHLD is ignored in another handler as then the
# shell will complain:
#
#	./tsigtrap.sh: got sig 20, but no trap command set!


trap "rc=\$?;
 echo \"pid \$\$: EXIT handler fired\";
 ${EXTRA_TRAP_CODE}
 exit \$rc" 0

trap "trap - HUP;
 echo \"pid \$\$: HUP handler fired\";
 ${EXTRA_TRAP_CODE}
 kill -HUP \$\$" HUP

trap "trap - INT;
 echo \"pid \$\$: INT handler fired\";
 ${EXTRA_TRAP_CODE}
 kill -INT \$\$" INT

trap "trap - QUIT;
 echo \"pid \$\$: QUIT handler fired\";
 ${EXTRA_TRAP_CODE}
 kill -QUIT \$\$" QUIT

trap "trap - PIPE;
 echo \"pid \$\$: PIPE handler fired\";
 ${EXTRA_TRAP_CODE}
 kill -PIPE \$\$" PIPE

trap "trap - TERM
 echo \"pid \$\$: TERM handler fired\";
 ${EXTRA_TRAP_CODE}
 kill -TERM \$\$" TERM

# XXX This won't always be seen if termination is due to a signal
#
trap "rc=\$?; trap - CHLD;
 echo \"pid \$\$: CHLD handler fired, \$!: exit code: \$rc\";
 ${EXTRA_TRAP_CODE}" CHLD

trap

echo "$0: now generate signal of your choice, or type EOF...."

# WARNING:  You can't send the signal from another terminal session unless you
# also kill this cat with it too as the shell will still be waiting for the cat
# to die even though it has received the signal....  (from a terminal session
# all processes in the terminal foreground group will get the same signal, so
# the cat dies as the shell gets the signal)
#
# XXX the trap handlers can't kill the cat, even if they could figure out its
# PID, as they won't run until after the cat dies.

cat
#/usr/bin/false

lastpid=$!

rc=$?

#trap - HUP INT QUIT PIPE TERM

echo "NORMAL EXIT"
echo "$0: PID $$: exit code $rc"
if [ -n "${lastpid}" ]; then
	echo "$0: last PID: ${lastpid}"
fi
exit $rc

# use C-c C-k to send SIGINT in the compile buffer....
#
# (don't run this with "ksh -i" or C-c C-k won't work)
#
# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (concat "sh ./" (file-name-nondirectory (buffer-file-name)))
# End:
