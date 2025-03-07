.PHONY: all
all:
	@env | sort

#
# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (let ((fn (file-name-nondirectory (buffer-file-name)))) (concat "VAR=environ " (default-value 'compile-command) " -f " fn))
# End:
#
