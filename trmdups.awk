#!/usr/bin/awk -f

BEGIN {
	RS=":";
}
{
	sub(sprintf("%c$", 10), "");
	if (! A[$0]) {
		A[$0]=1;
		printf("%s%s", ((NR == 1) ? "" : ":" ), $0);
	}
}

# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (concat "echo $PATH; echo $PATH | awk -f " (file-name-nondirectory (buffer-file-name)))
# End:
