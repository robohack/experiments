#include <stdint.h>
#include <stdio.h>
#include <signal.h>

int main(void);

/*
 * XXX must be compiled with -DUSE_VOLATILE to qualify nonatomic_t with
 * "volatile" for correctness in C89 or newer.
 */

/*
 * defines an integer type 'nonatomic_t' and declares a variable 'x' using it
 *
 * also defines an identical-sized type 'testint_t' that will never be
 * qualified as volatile.
 */
#include "tatomic.h"

int
main()
{
	/*
	 * catch_signal() will increment the external variable 'x'
	 */
	(void) signal(SIGUSR1, catch_signal);

	while (1) {
		register testint_t	nvtmp;
		register nonatomic_t	tmp;

		/*
		 * all of the following statements will be optimized away if
		 * nonatomic_t is qualified with "volatile" (i.e.this is
		 * compiled with "-DUSE_VOLATILE")
		 */

		/* XXX no sequence point -- may always be optimized away */
		if (!(x == x)) {
			/*
			 * XXX with GCC 4.1.x and 4.2.x this statement will
			 * always be optimized away, if nonatomic_t is not
			 * qualified as volatile (i.e. if compiled without
			 * USE_VOLATILE), even if '-O0' is used.
			 *
			 * I.e. GCC is "smart" enough to always eliminate code
			 * bounded by tautological tests, and this is not a
			 * feature of the optimizer, per se.
			 *
			 * XXX however GCC _should_ probably eliminate this
			 * even when 'x' is qualified as "volatile" because
			 * there are no sequence points in the expression, yet
			 * it does not.
			 */
			puts("x does not equal X!");
		}

		if (tmp = x, !(tmp == x)) {
			puts("tmp does not equal X!");
		}

		if (nvtmp = x, !(nvtmp == x)) {
			puts("nvtmp does not equal X!");
		}

		/* XXX no sequence point -- may always be optimized away */
		if ((x - x)) {
			/*
			 * XXX, as above.
			 *
			 * Interestingly GCC generates the same machine code
			 * (for x86(_64) at least) for this expression as it
			 * does for the one above.  PCC does not of course.
			 */
			puts("x - x does not equal 0!");
		}

		if (tmp = x, (tmp - x)) {
			puts("tmp - X does not equal 0!");
		}

		if (nvtmp = x, (nvtmp - x)) {
			puts("nvtmp - X does not equal 0!");
		}
	}
	/* NOTREACHED */
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " OPTIM='-O3' CPPFLAGS='-DUSE_VOLATILE' LDLIBS=tatomic-sigusr.c " fn " && echo 'send SIGUSR1 while running'"))
 * End:
 */
