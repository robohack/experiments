#include <stdio.h>
#include <pwd.h>

int
main(argc, argv)
	int	argc;
	char	*argv[];
{
	struct passwd	*pwd;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s user\n", argv[0]);
		exit(2);
	}
	if (!(pwd = getpwnam(argv[1]))) {
		fprintf(stderr, "%s: no such user %s.\n", argv[0], argv[1]);
		exit(1);
	}
	printf("%s: %s.\n", argv[1], pwd->pw_gecos);

	exit(0);
}
