#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

int main(void);

int
main()
{
	int	foo = -2;
	short	sfoo = -2;
	unsigned int u_var = ~(UINT_MAX & INT_MAX);
	int s_var = ~(UINT_MAX & INT_MAX);

	printf("uvar: (int) %d (uint) %u (hex) 0x%x\n", u_var, u_var, u_var);
	printf("svar: (int) %d (uint) %u (hex) 0x%x\n", s_var, s_var, s_var);

	printf("foo = (int) %d.\n", foo);
	printf("foo = (unsigned) %u.\n", foo);
	printf("foo = (hex) %x.\n", foo);
	printf("foo = (octal) %o.\n", foo);

	printf("sfoo = (int) %d.\n", sfoo & 0xffff);
	printf("sfoo = (unsigned) %u.\n", sfoo & 0xffff);
	printf("sfoo = (hex) %x.\n", sfoo & 0xffff);
	printf("sfoo = (octal) %o.\n", sfoo & 0xffff);

	exit(0);
	/* NOTREACHED */
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
