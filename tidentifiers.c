/*
 * typedefs, variables, and functions share the same namespace
 *
 * struct tags are in their own namespace
 *
 * note:  you still need the struct tag to provide forward declarations for
 * anonymous struct pointers.
 */

struct s;
//typedef struct s t;

typedef struct s {
	int x;
} t;

//int x;

void x(void);
void x()
{
	return;
}

//int s;

void s(void);
void s()
{
	return;
}

//int t;  	/* error: 't' redeclared as different kind of symbol */

void t(void);	/* error: 't' redeclared as different kind of symbol */
void t()
{
	return;
}


/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " " fn ".o"))
 * End:
 */
