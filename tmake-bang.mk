foo!= echo bar#baz
all:
	echo '<${foo}>'
#
# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (let ((fn (file-name-nondirectory (buffer-file-name)))) (concat (default-value 'compile-command) " -f " fn))
# End:
#
