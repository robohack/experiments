#include <float.h>
#include <stdio.h>
#include <stdlib.h>

int main(void);

int
main()
{
	//char d[10] = {0x13, 0x43, 0x9b, 0x64, 0x28, 0xf8, 0xff, 0x7f, 0x00, 0x00};
	//long double rd = *(long double*)&d;
	long double rd = 3.3621e-4932L;

	printf("DBL_MIN = %%e\t %e (test = %e)\n", DBL_MIN, 1.0);
	printf("LDBL_MIN = %%Le\t %Le (test = %e)\n", LDBL_MIN, 1.0);

	printf("DBL_MIN = %%g\t %g (test = %g)\n", DBL_MIN, 1.0);
	printf("LDBL_MIN = %%Lg\t %Lg (test = %g)\n", LDBL_MIN, 1.0);

	printf("DBL_MIN = %%a\t %a (test = %a)\n", DBL_MIN, 1.0);
	printf("LDBL_MIN = %%La\t %La (test = %a)\n", LDBL_MIN, 1.0);

	printf("3.3621e-4932L =\t %Le (test = %e)\n", 3.3621e-4932L, 1.0);

	printf("rd =\t\t %Le (test = %e)\n", rd, 1.0);		/* 3.362100e-4932 */

	printf("rd * 4 =\t %Le (test = %e)\n", rd * 4.0, 1.0);

	printf("3.3622e-4932L =\t %Le (test = %e)\n", 3.3622e-4932L, 1.0);

	exit(0);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
