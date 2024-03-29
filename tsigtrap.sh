#!/bin/sh

# note this causes problems with catching a signal when not run from an
# interactive session (e.g. when the 'compile' command is used in emacs...
#
# Also it is not necessary for 'trap CHLD' -- it just makes process exits
# verbose...
#
#set -o monitor

value=0

echo "value = ${value}...."

# In order to pass values of variables into trap command lists with the values
# at the time of the trap creation one must take care to escape variable
# expansions which are expected to be expanded at trap execution time, as well
# as embedded double quotes.  This is is because the "action" string is split,
# parsed, and expanded, first when the trap command is run to set the handler,
# and then again when the handler is triggered and executed.

# One must take care not to set the EXIT (0) trap to '-' or '' within its
# handler if one wants it to also fire when the those shells which do support
# doing so exit on the signal the other traps send to it....
#
# Note:  However only ksh93 and Bash will execute the EXIT handler when the
# shell sends itself a SIGHUP, SIGINT, (SIGPIPE?,) or SIGTERM signal, even if
# the trap handler has set the action to supposedly be the "default" action
# (which for all those SHOULD be to cleanly terminate the process).

# Note:  Oddly ksh93 won't trigger the CHLD handler if the 'cat' process exits
# normally (i.e. when 'cat' reads EOF and exits) -- apparently it only monitors
# background and non-interactive processes.

# Note:  Passing on the same signal received to the shell, after clearing the
# trap handler for that signal, as this example does, is probably not always
# what's wanted.  Just use "exit", or maybe pass SIGTERM, instead.

# note: one can also call additional code, including a function defined in the
# script from a trap handler, and use variables set after the trap is defined.
#
demo ()
{
	echo "this is a function!"
}
EXTRA_TRAP_CODE="demo; echo \"pid \$\$: value = \${value}\";"

value=${RANDOM:-99999}

# Hmmm... it seems NOT all shells with $RANDOM have 'trap ERR', since $RANDOM is
# now POSIX, but 'trap ERR' is not....
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
 echo \"pid \$\$: EXIT handler fired, exit code: \$rc\";
 ${EXTRA_TRAP_CODE}
 exit \$rc" 0			# only heirloom sh doesn't have this as EXIT?

trap "rc=\$?; trap - HUP;
 echo \"pid \$\$: HUP handler fired, exit code: \$rc\";
 ${EXTRA_TRAP_CODE}
 kill -HUP \$\$" HUP

trap "rc=\$?; trap - INT;
 echo \"pid \$\$: INT handler fired, exit code: \$rc\";
 ${EXTRA_TRAP_CODE}
 kill -INT \$\$" INT

# n.b.:  this variant is an example of using a normal exit which will trigger
# the EXIT handler in all known shells
#
# Note though that all shells lose the exit code in this case as it seems $? is
# not set when the handler fires in this particular case, though how/why I
# cannot understand.  However most shells, i.e. Heirloom Sh, Bash, pdksh, ksh93,
# bosh, and pbosh pass the rc=0 value through intact to the EXIT handler, while
# Ash (sh, dash, etc.), sets $? to the signal exit value (130 in the case of
# SIGINT) in or before executing the EXIT handler.
#
#trap "rc=$?; trap - INT;
# echo \"pid \$\$: INT handler fired, exit code: \$rc\";
# ${EXTRA_TRAP_CODE}
# exit $rc" INT

trap "rc=\$?; trap - QUIT;
 echo \"pid \$\$: QUIT handler fired, exit code: \$rc\";
 ${EXTRA_TRAP_CODE}
 kill -QUIT \$\$" QUIT

trap "rc=\$?; trap - PIPE;
 echo \"pid \$\$: PIPE handler fired, exit code: \$rc\";
 ${EXTRA_TRAP_CODE}
 kill -PIPE \$\$" PIPE

trap "rc=\$?; trap - TERM
 echo \"pid \$\$: TERM handler fired, exit code: \$rc\";
 ${EXTRA_TRAP_CODE}
 kill -TERM \$\$" TERM

# XXX This won't always be seen if termination is due to a signal
#
trap "rc=\$?; trap - CHLD;
 echo \"pid \$\$: CHLD handler fired, \$!: exit code: \$rc\";
 ${EXTRA_TRAP_CODE}" CHLD

if [ ${RANDOM:-0} -ne ${RANDOM:-0} ] ; then
	# xxx and hopefully "trap -p" too?
#	trap -p 
	trap
else
	trap
fi

# XXX we should test saving traps, including with '-p' if available

# XXX we should test using traps in functions too

echo "$0: now generate signal of your choice, or type EOF...."

# WARNING:  You can't send the signal from another terminal session unless you
# also kill this cat with it too as the shell will still be waiting for the cat
# to die even though it has received the signal....  (from a terminal session
# all processes in the terminal foreground group will get the same signal, so
# the cat dies as the shell gets the signal)
#
# XXX the trap handlers can't kill the cat, even if they could figure out its
# PID, as they won't run until after the cat dies.

# N.B. WARNING:  mksh (at least R56) seems to run the cat without attatching its
# stdin to the tty, _AND_ it ignores useful signals despite the traps!  It must
# be sent SIGKILL!

cat
#/usr/bin/false

# XXX Heirloom Sh is quite interesting!
#
# Here, in certain circumstances, it says somewhat surprisingly "$history not
# set" (surprising because that string is not in the sh binary).
#
# Then it goes on to execute the INT handler, then the CHLD handler twice, and
# IFF the originally triggered handler was INT and the variant calling exit is
# used, then finally the EXIT handler, else it continues executing after the cat
# command (which was also killed by the signal if via the TTY driver),.
#
# So, the "trap - SIG" calls in the handlers don't set the default action, but
# set the trap action to "-", which in certain circumstances just happens to be
# in $PATH as a link to the "rc" shell, and running "-" alone does indeed cause
# that thing to say "$history not found"!
#
# Note all of this does match the manual, and indeed bosh(1) says "The ‐
# argument was not supported in older versions of sh."
#
# N.B.:  So, "trap - SIG" is not really distantly backward-compatible.
#
lastpid=$!

rc=$?

#trap - HUP INT QUIT PIPE TERM

echo "NORMAL EXIT"
echo "$0: PID $$: exit code $rc"
if [ -n "${lastpid}" ]; then
	echo "$0: last PID: ${lastpid}"
else
	echo "$0: '\$!' was empty!"
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
