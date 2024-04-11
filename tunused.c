#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>

/*
 * re-declare printf() so that we always get an explicit warning
 */
int	printf(const char * __restrict, ...) __attribute__((__warn_unused_result__));

struct tst {
	int intern_err;
	const char *err_info;
};

static int
test_unused_detection(struct tst *this)
{
	/*
	 * gcc-4.1.3 (netbsd-5) does complain
	 * gcc-4.1.2 (netbsd-4) does complain
	 *
	 * gcc-4.2.1 (LLVM, Apple 10.8.2, Xcode 4.5.2) does not complain
	 * clang 4.1 (Apple 10.8.2, Xcode 4.5.2) does not complain
	 *
	 * gcc-4.0.1 (Apple 10.6.8, Xcode 3.2.6) does complain
	 * gcc-4.2.1 (Apple 10.6.8, Xcode 3.2.6) does complain
	 * gcc-4.7.2 (Apple 10.6.8, Xcode 3.2.6) does complain
	 * clang 1.7 (Apple 10.6.8, Xcode 3.2.6) does not complain
	 */
	char *str;			/* not strictly unused */
	const char *s;
	int res = 0;

	if (this->intern_err) {
		s = strerror(this->intern_err);
	} else {
		str = malloc((size_t) BUFSIZ);
		s = strerror(this->intern_err);
		strcat(str, "holy cow! ");
		strncat(str, s, (size_t) (BUFSIZ-1));
	}
	if (s) {
		res = printf("s = %s\n", s);
	}
	if (!this->intern_err) {
		free(str);
	}
	return res;
}


int	main(int, char *[]);

int
main(int argc		__attribute__((__unused__)),
     char *argv[])
{
	int res;
	const char *msg = "error string: %m";
	const char *argv0 = "progname";

	(void) argc;	/* this works just as well as __attribute__((__unused__)) */

	argv0 = (argv0 = strrchr(argv[0], '/')) ? argv0 + 1 : argv[0];

	/* gcc (all versions?) and clang complain here */
	printf("%s: (%s) %s (%s)\n", argv0, getlogin(), "testing", msg);

	/* only gcc (all versions?) complain here */
	(void) printf("%s: (%s) %s (%s)\n", argv0, getlogin(), "testing", msg);

	/* unused-var-testing */ {
		struct tst ei = { 0 };

		if (argv[1] && strcmp(argv[1], "doit") == 0) {
			ei.intern_err = EINPROGRESS;
			ei.err_info = "FUBAR NONE";
		}

		res = printf("tst = %d\n", test_unused_detection(&ei));
	}

	exit(0);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
