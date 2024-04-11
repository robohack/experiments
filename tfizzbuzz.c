#include <stdio.h>
#include <stdlib.h>

int main(void);

int
main()
{
	int i;

	printf("Clear coding:\n");
	for (i = 1; i <= 100; i++) {
		/* if (i % 3 == 0 && i % 5 == 0) { */
		if (i % 15 == 0) {
			printf("FizzBuzz\n");
		} else if (i % 3 == 0) {
			printf("Fizz\n");
		} else if (i % 5 == 0) {
			printf("Buzz\n");
		} else {
			printf("%d\n", i);
		}
	}

	/*
	 * it looks like GCC does a wee bit better with this variant, both with
	 * and without optimization.  Note: Go doesn't have conditional exprs.
	 */
	printf("Idiomatic coding:\n");
	for (i = 1; i <= 100; i++) {
		(void) ((i % 15 == 0) ? printf("FizzBuzz\n") :
			((i %  3 == 0) ? printf("Fizz\n") :
			 ((i %  5 == 0) ? printf("Buzz\n") :
			  printf("%d\n", i))));
	}

	exit(0);
}


/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
