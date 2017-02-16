#! /bin/ksh93

# XXX WARNING!!! XXX
#
# 'trap CHLD' may not work reliably because you can lose some signals if more
# than one arrives before the handler finishes.
#
# best is probably:  http://stackoverflow.com/a/1048390/816536

# N.B.:  This is necessary for Bash, but not ksh93
#
set -o monitor

echo "$0: PID $$: starting.... wait 6 seconds..."

sleep 2 &
pids=$!
echo "$0: started child process: $!"

sleep 4 && exit 1 &
pids="$pids $!"
echo "$0: started child process: $!"

sleep 5 && exit 2 &
pids="$pids $!"
echo "$0: started child process: $!"

sleep 6 &
pids="$pids $!"
echo "$0: started child process: $!"

# for testing SIGSTOP, etc.:
#
#	twatcher.sh: CHLD trap fired (3918): 0
#	13863 exited OK
#	742 is still alive
#	
# sent SIGSTOP to 742:
#	twatcher.sh[70]: wait: 742: Stopped (SIGSTOP)
# sent SIGCONT to 742:
# sent SIGTERM to 742:
#	twatcher.sh: CHLD trap fired (3918): 271
#	twatcher.sh[70]: wait[52]: wait: 742: Terminated
#	742 exited with non-zero exit status: 271
#	
#	twatcher.sh: exit status: 127
#
#sleep 30 &
#pids="$pids $!"
#echo "$0: started child process: $!"

echo

checkpids ()
{
	lrc=$?
	cpid=$1
	rc=$2
	echo "$0: CHLD trap fired ($cpid): $rc"
	if [ $rc != $lrc ]; then
		echo "$0: caller said rc=$rc, we have $lrc" 2>&1
	fi
	if [ $$ != $cpid ]; then
		echo "$0: caller said CPID=$cpid, we have $$" 2>&1
	fi

	for pid in $pids; do
		if kill -0 $pid 2>/dev/null; then
			echo "$pid is still alive"
		elif wait $pid; then
			echo "$pid exited OK"
# only bash and ksh93 can do pattern substitution
			pids=${pids/$pid/}
# doing this in pdksh and sh causes weirdness
#			pids=$(echo $pids | sed s/$pid//)
		else
			# note: this shows the exit status of the "wait $pid" above
			echo "$pid exited with non-zero exit status: $? ($rc)" 2>&1
# only bash and ksh93 can do pattern substitution
			pids=${pids/$pid/}
# doing this in pdksh and sh causes weirdness
#			pids=$(echo $pids | sed s/$pid//)
		fi
	done
	echo
}

# $? works for ksh93 and pdksh, but $$ does not
#
# neither work for sh or bash
#
# (but $? from the 'wait' works in the function for bash and pdksh, as well as ksh93!)
#
# this does not work properly (dynamically) for pdksh or sh
#
trap 'checkpids $$ $?' CHLD

wait $pids

rc=$?

echo "$0: exit status: $rc"

exit $rc
