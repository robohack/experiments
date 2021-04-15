#!/usr/bin/awk -f

function parsehex(V)
{
	num = 0;

	if (V ~ /^0[xX]/) {
		V = substr(V, 3);
	}

	for (N = 1; N <= length(V); N++) {
		num = (num * 16) + H[substr(V, N, 1)];
	}

	return(num);
}

BEGIN {
	# set up for parsehex()
	H[""] = 0;
	for (N = 0; N < 16; N++) {
		H[sprintf("%x", N)] = N;
#		print "H[", sprintf("%x", N), "] =", N;
		H[sprintf("%X", N)] = N;
#		print "H[", sprintf("%X", N), "] =", N;
	}

	# example:
	RAND_MAX = parsehex("0x7FFFFFFF");
	print RAND_MAX;

	RAND_MAX = parsehex("0x7fffffff");
	print RAND_MAX;

	RAND_MAX = parsehex("0X7fffffff");
	print RAND_MAX;

	RAND_MAX = parsehex("7fffffff");
	print RAND_MAX;
}


# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (concat "awk -f " (file-name-nondirectory (buffer-file-name)))
# End:
