#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pwd.h>

int main(int, char *[]);

int
main(argc, argv)
	int	argc;
	char	*argv[];
{
	const char *argv0 = "tgetpwnam";
	char *user;
	struct passwd	*pwd;

	argv0 = (argv0 = strrchr(argv[0], '/')) ? argv0 + 1 : argv[0];

	if (argc == 2) {
		user = argv[1];
	} else if (argc == 1) {
		if ((user = getenv("LOGNAME")) == NULL) {
			fprintf(stderr, "%s: you don't seem to exist (%s)\n", argv0, strerror(errno));
			exit(1);
		}
	} else {
		fprintf(stderr, "Usage: %s [user]\n", argv0);
		exit(2);
	}
	if (!(pwd = getpwnam(user))) {
		fprintf(stderr, "%s: no such user %s: %s.\n", argv0, user, strerror(errno));
		exit(1);
	}
	printf("%s: %s (%s) is %ld:%ld, with shell '%s'.\n",
	       argv0,
	       user,
	       pwd->pw_gecos,
	       (long int) pwd->pw_uid,
	       (long int) pwd->pw_gid,
	       pwd->pw_shell);

	exit(0);
	/* NOTREACHED */
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
