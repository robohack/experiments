#!/usr/bin/awk -f

function writeblock(val, i)
{
	# N.B.:  only gawk and mawk can form internal strings with embedded NUL
	# bytes (though awk can print them with '%c')
	#
	char = sprintf("%c", int(val));

#	printf("writing block#%d: '%s' (%c)\n", val, char, val) > "/dev/stderr";

	blk = "";
	for (i = 0; i < 512; i++) {
		blk = blk char;
	}
	printf("%*s", 512, blk);
}

function writebyteblocks()
{
	CHAR_BIT = 8;

	for (i = 0; i < 2^CHAR_BIT; i++) {
		writeblock(i);
	}
}

BEGIN {
	while (1) {
		writebyteblocks();
	}
}

# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (concat "awk -f " (file-name-nondirectory (buffer-file-name)) " | dd count=1024 | od -c")
# End:
