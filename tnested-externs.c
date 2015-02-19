/*
 * Possible dangers of "extern" declarations encountered in a function
 *
 * (linkage rules do not always have entirely obvious implications)
 *
 * Rule of thumb:  All external declarations should occur in header files, and
 * the header file should be included to reference the "global" definition.
 */

void f1(void);
void f1()
{
	extern int a;			// external linkage

	(void) a;
}


int b;					// external linkage
void f2(void);
void f2()
{
	extern int b;			// also (same) external linkage

	(void) b;
}


static int c;				// internal linkage
void f3(void);
void f3()
{
	extern int c;			// also (same) _internal_ linkage
					// (perhaps the most dangerous one!)
					//
					// If there is a visible prior
					// declaration that specifies some kind
					// of linkage, then it simply uses it.

	(void) c;
}


static int d;				// internal linkage
void f4(void);
void f4()
{
	double d;			// no linkage (-Wshadow gives warning)

	(void) d;

	/* GCC:  error: variable previously declared 'static' redeclared 'extern' */ {
		extern int d;		// external linkage: undefined behavior
					// (-Wshadow gives warning)

		(void) d;
	}
}



/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat (default-value 'compile-command) " " fn ".o"))
 * End:
 */
