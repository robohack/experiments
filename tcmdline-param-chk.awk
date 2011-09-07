#
# usage:  awk -v X=something -f tcmdline-param-chk.awk
#
BEGIN {
	#
	# this works because undefined variables are both empty strings and
	# zero numerical values at the same time, but never otherwise.
	#
	# If set to an empty string on the command line then it will somehow
	# not have the numerical value of zero, though it will print as zero
	# when referenced as a number.  This seems like magic, though my guess
	# is that it shows up internally with a string pointer of NULL unless
	# it has been set in any way, and a "defined" empty string is not a
	# NULL pointer, but rather a pointer to a NUL.  :-)
	#
	# If set to zero on the command line then it will not an empty string
	# as it will print as "0".
	#
	# Amazingly though not strictly defined in the AWK book, this works in
	# exactly the same way for all known implementations.
	#
	# Note the BEGIN section here happens after command-line parameters are
	# processed, so default values for command-line parameters cannot be
	# naively set without using this test:
	#
	if (X == 0 && X == "") { 
		printf("X is undefined (X = %d, '%s')\n", X - 0, X "");
	} else if (X == "") {
		#
		# the string test must be first, before the number test
		#
		printf("if defined as a string, X cannot be empty! (X = %d, '%s')\n", X - 0, X "");
		exit 1;
	} else if (X == 0) {
		printf("if defined as a number, X cannot be zero! (X = %d, '%s')\n", X - 0, X "");
		exit 1;
	} else {
		printf("X = %d, '%s'\n", X - 0, X "");
	}
}
