#include <stdio.h>

#define lput(str) fprintf(fp, "%s\n", str)

#define M(x) x ## _

#define F2	M(f2)

#define	MKS(x) _MKS(x)
#define _MKS(x) #x

#define SZLONG	32
#define SZCHAR	8

#if (SZLONG / SZCHAR) == 2
# define SILONG 2
#elif (SZLONG / SZCHAR) == 4
# define SILONG 4
#elif (SZLONG / SZCHAR) == 8
# define SILONG 8
#else
# error "Unknown size combinations!"
#endif

void F2(void);

void
F2()
{
	FILE	*fp = stdout;

	lput("");
	lput("foobar.");
	lput("nutbar."                        );
	lput("foobar.");
	lput(MKS(SZLONG/SZCHAR));
	lput(MKS(SILONG));
}

int main(void);

int
main()
{
	F2();
	return 0;
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
