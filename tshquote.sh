#
#	Quote args to make them safe in the shell.
#
# for systems without shquote(1), or printf(1)'s "%q"
#
# See trunningsh.sh for how to use $@ and IFS to avoid having to re-quote
# things.
#
# See also tstringlist.sh for a method of saving quoted strings in individual
# variables and creating a list of those variables; and which works entirely
# without printf(1) and sed(1).
#
#
# Usage: quotedlist=$(shell_quote args...)
#
# e.g.:  quotedlist=$(shell_quote *.pdf)	# filenamess with spaces
#
# or:    quotedlist=$(shell_quote "$@")
#
# After building up a quoted list, use it by evaling it inside
# double quotes, like this:
#
#	eval "set -- $quotedlist"
#	for str in "$@"; do
#		# fiddle "${str}"
#	done
#
# or like this:
#
#	eval "\$a_command $quotedlist \$another_parameter"
#
shell_quote()
{
	local result=''
	local arg
	for arg in "$@" ; do
		#
		# Append a space to our result, if necessary
		#
		result=${result}${result:+ }

		# Convert each embedded ' to \' , then insert ' at the
		# beginning of the line, and append ' at the end of
		# the line.
		#
		# using sed:
		#
		result=${result}$(printf "%s\n" "$arg" | \
			sed -e "s/'/'\\\\''/g" -e "1s/^/'/" -e "\$s/\$/'/")

		# alternate using Awk:
		#
		# NOTE: we cannot avoid the printf(1) pipeline if we
		# hope to deal with strings that might contain double
		# quotes.  (maybe printf is builtin!)
		#
		#result=${result}$(printf "%s\n" "$arg" | awk '
		#	{
		#		gsub(/'"'"'/, "\\'"'"'", s);
		#		printf("'"'"'%s'"'"'\n", s);
		#	}
		#' < /dev/null)
	done

	# use printf(1) instead of echo to avoid weird "echo"
	# implementations.
	#
	printf "%s\n" "$result"
}
#
#
# for a single string at a time:
#
sh_quote()
{
	printf %s\\n "$1" | sed "s/'/'\\\\''/g;1s/^/'/;\$s/\$/'/"
}
