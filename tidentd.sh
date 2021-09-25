#!/bin/sh
#

if [ "$1" != sub ] ; then
	export HOST=$(host $1 | awk '{ print $3; }')
	declare -i I=1
	export PPPID=$$

	while [ $I -lt 1024 ] ; do
		(
			export I
			socket -r -p "$0 sub" $HOST $I 2>/dev/null
		)

		I=$(($I + 1))
	done
else
	LOCAL=$(netstat -n | grep ":.\+$HOST:$I.\+EST" | awk '{print $4;}' | cut -d: -f2)
	echo "$I,$LOCAL" | nc -w 1 $HOST auth > /proc/$PPPID/fd/1
	kill $PPID
fi
