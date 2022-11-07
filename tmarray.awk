#!/bin/sh
# -*- awk -*-

# xxx only GAWK (since 4.0) has true multi-dimensional arrays

# Magic to allow an AWK script to use the first "gawk" in $PATH:
#
some_arbitrary_awk_variable=0 "eval" "exec /usr/bin/env" "gawk -f $0 -- ${@}"

# The real AWK code starts here...
#
# (though AWK will of course parse the line above OK, and then execute it for
# any and all input records)
#
# XXX this trick can only work in GNU AWK since it is the only one which stops
# processing its own command-line options after the (last?) '-f'.

BEGIN {
	delete arr["foo"]["bar"];

	# example array where each schema has matching table names
	#
	arx["t1"]["s1"]
	arx["t2"]["s1"]
	arx["t3"]["s1"]
	arx["t4"]["s1"]

	arx["t1"]["s2"]
	arx["t2"]["s2"]
	arx["t3"]["s2"]

	arx["t1"]["s3"]
	arx["t2"]["s3"]

	# real-world array where each schema has a unique table name
	#
	arr["s1"]["t1a"]
	arr["s1"]["t2b"]
	arr["s1"]["t3c"]
	arr["s1"]["t4x"]

	arr["s2"]["t1d"]
	arr["s2"]["t2e"]
	arr["s2"]["t3f"]

	arr["s3"]["t1g"]
	arr["s3"]["t2h"]

}

END {
	# If each schema has (some of the) matching table names...
	#
	for (a in arx) {
		for (b in arx[a]) {
			printf("%s.%s\t", b, a);
		}
		printf("\n");
	}
	printf("\n");

	m = 0;
	c = 0;

	# If each schema has unique table names...
	#
	# - this first naive way prints one schema per row
	#
	for (a in arr) {
		x = 0;
		c++;
		for (b in arr[a]) {
			printf("%s.%s\t", a, b);
			x++;
		}
		if (x > m) {
			m = x;
		}
		printf("\n");
	}
	printf("\n");

	printf("schemas:\t %d\n", c);
	printf("max-tables:\t %d\n", m);
	printf("\n");

	# to get one schema per column we must "rotate" the array
	#
	# - here this is done by creating two separate integer indexed arrays,
	# one containing the list of schema names, the other being the 2d array
	# of table names, with a primary index equal to that of the schema names
	#
	delete n[0];
	m = 0;
	c = 0;
	i = 0;
	for (a in arr) {
		n[i] = a;
		j = 0;
		x = 0;
		c++;
		# xxx mystery:  why does the first schema seem out of order?
		for (b in arr[a]) {
			t[i][j++] = b;
			x++;
		}
		if (x > m) {
			m = x;
		}
		i++;
	}

	# finally we traverse the new 2d array of table names "inside out" to
	# rotate it thus allowing us to print by the row but appear as if we
	# were printing columns
	#
	for (j = 0; j < m; j++) {
		for (i = 0; i < c; i++) {
			if (t[i][j] != "") {
				printf("%s.%s\t", n[i], t[i][j])
			}
		}
		printf("\n");
	}
	printf("\n");
}
# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (concat "gawk -f " (file-name-nondirectory (buffer-file-name)) " < /dev/null")
# End:
