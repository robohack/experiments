#!/bin/sh

while read foo ; do
	printf "%s\n" "$foo"
done <<- '__EOF__'
	This is a back\
		slash continued line
	this is not a backslash continued line
	another back\
	slash
	another not
__EOF__
