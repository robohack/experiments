#!/bin/sh
# -*- awk -*-

# Magic to allow an AWK script to use the first "gawk" in $PATH:
#
some_arbitrary_awk_variable==0 "eval" "exec /usr/bin/env" "gawk -f $0 -- $@"

# The real AWK code starts here...

BEGIN {
	TRIALS = 10000000
	curtime = systime()

	for (i = 1; i <= TRIALS; i++)
		rand()

	print TRIALS, "awk in ", systime() - curtime, " seconds"

	# AWKs will normally exit if there's no other pattern+action code blocks
	# than BEGIN{}, but the magic 'var=0 "eval"' line breaks this rule so:
	#
	exit;
}
