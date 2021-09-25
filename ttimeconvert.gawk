#!/bin/sh
# -*- awk -*-

# Magic to allow an AWK script to use the first "gawk" in $PATH:
#
some_arbitrary_awk_variable=0 "eval" "exec /usr/bin/env" "gawk -f $0 -- $@"

# The real AWK code starts here...

# XXX this can only work in GNU AWK since it is the only one which
#  offers the mktime() builtin function

# Here when given a complete date & time we print the corresponding year and
# ISO week number
#
# expects to be fed either mktime()-ready format,
# or ISO-like (YYYY-MM-DD HH:MM:SS [-+]HMM) format,
# or ISO-strict (yyyy-mm-dd%hh:mm:ss[-+]h:mm) format

{
	gsub(/[-+:T]/, " ", $0);
	printf("%s\n", strftime("%Y-W%V", mktime($0)));
}
