#include <locale.h>
#include <stdio.h>
#include <stdlib.h>

/* XXX these are the same macros as NetBSD's, and so must be identical */
#if defined(_U) && _U != 0x01
# error "_U != 0x01"
#elif !defined(_U)
# define _U	0x01
#endif
#if defined(_L) && _L != 0x02
# error "_L != 0x02"
#elif !defined(_L)
# define _L	0x02
#endif

/* xxx uses GCC "statement expressions" */
#if 0
#define	MY_ISALPHA(c)							\
	({								\
		const typeof(c) _c = (c);				\
		(sizeof(c) > 1 && (unsigned int) (_c) == (unsigned int) EOF) ? 0 : \
			((int) ((_ctype_ + 1)[(unsigned char) (_c)] & (_U|_L))); \
	})
#else
#define	MY_ISALPHA(c)							\
	({								\
		const typeof(c) _c = (c);				\
		const unsigned int _u = (unsigned int) (_c);		\
		(sizeof(c) > 1 && (_u) == (unsigned int) EOF) ? 0 :	\
			((int) ((_ctype_ + 1)[(unsigned char) (_c)] & (_U|_L))); \
	})
#endif

#define CORRECTP(expr)	((expr) ? " [correct]" : "[wrong]")

extern const unsigned char	*_ctype_;

static unsigned int
tunsigned_abs(unsigned int a,
              unsigned int b)
{
	return abs(a - b);
}

int main(void);

int
main()
{
	unsigned char uc = 'a';

	printf("MY_ISALPHA(uc) = 0x%x", MY_ISALPHA(uc));
	printf(" %s\n", CORRECTP(MY_ISALPHA(uc)));

	setlocale(LC_ALL, "en_US.ISO8859-1");

	printf("MY_ISALPHA(EOF) = 0x%x", MY_ISALPHA(EOF));
	printf(" %s\n", CORRECTP(!MY_ISALPHA(EOF)));

	printf("abs(2 - 3) = %u\n", tunsigned_abs(2, 3));

	exit(0);
	/* NOTREACHED */
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
