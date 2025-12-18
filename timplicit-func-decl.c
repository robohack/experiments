int
main(void)
{
	foo();

	return strlen("");
}

void foo(void)
{
	return;
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " CSTD=c99 " fn " && ./" fn))
 * End:
 */

