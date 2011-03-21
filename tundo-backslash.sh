#! /bin/sh

while read foo ; do
	print "$foo"
done < __EOF__
This is a back \
	slash continued line
this is not a backslash continued line
another back\
slash
another not
__EOF__
