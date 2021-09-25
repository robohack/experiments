#!/bin/sh

# a script to find the correct POSIX shell pathname

# Get list of paths to check
#
Sifs=$IFS
IFS=:
set $(getconf PATH)
IFS=$Sifs

# Check each path for 'sh'
#
for i in "$@" ; do
	if [ -x ${i}/sh ]; then
		Pshell=${i}/sh
		break
	fi
done

echo $Pshell
