#! /bin/sh
#
# save_args -- keep a list of quoted strings for later use via "$@"
#
# Note: creates a variable for each value, and uses these in another
# variable which can then be expanded with "eval" to re-create the
# original parameters, preserving words containing whitespace
#
# See also tshquote.sh for shell_quote() which can do the same, but
# produces its result on its stdout, and which does this simply using
# the normal single-quote characters.
#
# HOWEVER, shell_quote() requires printf(1) and sed(1), while this
# variant does it all internally to the shell.
#
save_args()
{
	local a c n
	c=0; n=$1; shift

	for a in "$@"; do
		c=$(($c+1))
		eval $n=\"\$$n \\\"\\\$${n}_\$c\\\"\"
		eval ${n}_$c=\"\$a\"
	done
}
#
# XXX  Need to write free_args() to unset all the ${n}_$c variables!!!
#
# use:
#
#	save_args list -foo bar "arg with spaces" "don't panic" '"Help!"'
#
# or:
#
#	save_args list *.pdf	# filenames with whitespace
#
# later:
#
#	eval "gv ${list}"
#
# or, being a bit silly:
#
#	. ./tshquote.sh
#
#	eval set -- ${list}
#
#	for str in "$@"; do
#		q="$(shell_quote "$str")"
#		list2="${list2}${list2:+ }${q}"
#	done
