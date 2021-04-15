#!/usr/bin/awk -f

BEGIN {
	hfmt = "%-4s %-30s %10s %10s %8s\n";
	tfmt = "%-4s %-30s %10d %10d %8d\n";

	# XXX this only works in GNU Awk:
	# (not even in the True Awk with gensub()!)
	#dfmt = gensub(/%([0-9]*)s/, "%\\1d", "g", hfmt);

	dfmt = hfmt;
	gsub(/%10s/, "%10d", dfmt);
	gsub(/%8s/, "%8d", dfmt);

	if (tfmt != dfmt) {
		printf("Wrong: %s", dfmt);
	}
}

# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (concat "gawk -f " (file-name-nondirectory (buffer-file-name)))
# End:
