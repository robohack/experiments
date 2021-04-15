#!/usr/bin/awk -f

# Simulate echo(1), from awk(1)

BEGIN {
	for (i = 1; i < ARGC; i++) {
		printf("%s ", ARGV[i]);
	}
	printf "\n"
	# awk will exit if there's no other pattern+action code blocks than BEGIN{}
}

# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (concat "awk -f " (file-name-nondirectory (buffer-file-name)) " 'hello' 'world'")
# End:
