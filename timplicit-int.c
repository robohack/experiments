foo(x)
{
	return x;
}

main()
{

	return foo(0);
}


/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " CSTD=c90 " fn " && ./" fn))
 * End:
 */

