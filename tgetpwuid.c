#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pwd.h>
#include <unistd.h>

int main(int, char *[]);

int
main(argc, argv)
	int	argc;
	char	*argv[];
{
	const char *argv0 = "tgetpwuid";
	uid_t uid;
	struct passwd	*pwd;

	argv0 = (argv0 = strrchr(argv[0], '/')) ? argv0 + 1 : argv[0];

	if (argc == 2) {
		uid = (uid_t) atoi(argv[1]);
	} else if (argc == 1) {
		uid = getuid();
	} else {
		fprintf(stderr, "Usage: %s [user]\n", argv0);
		exit(2);
	}
	if ((pwd = getpwuid(uid)) == NULL) {
		fprintf(stderr, "%s: no such user %ld: %s.\n", argv[0], (long int) uid, strerror(errno));
		exit(1);
	}
	printf("%s: %ld:%ld is %s (%s), with shell '%s'.\n",
	       argv0,
	       (long int) pwd->pw_uid,
	       (long int) pwd->pw_gid,
	       pwd->pw_name,
	       pwd->pw_gecos,
	       pwd->pw_shell);

	exit(0);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
