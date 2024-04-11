#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <pwd.h>
#include <unistd.h>

uid_t	real_uid;

unsigned short int	usi;
unsigned int		ui;
unsigned long int	uli;

int main(int, char *[]);

/* ARGSUSED */
int
main(argc, argv)
	int argc;
	char *argv[];
{
	char buf[BUFSIZ];
	float t;
	int ic;
	char who[BUFSIZ];

	if (argc > 1) {
		ic=10;
		printf("Parsing $1('%s') as '%%.6g'\n", argv[1]);
		/* xxx hmmmm... why did I do this???? */
		sscanf(argv[1], "%.6g", &ic);
		printf("ic=%d\n", ic);
	}
	while (fgets(buf, BUFSIZ, stdin) != NULL) {
		/* XXX sizeof(who) == 1024 on some platforms */
		if (sscanf(buf, "%f %d%1023s", &t, &ic, who) != 3) {
			sscanf(buf, "%f %1023s", &t, who);
			printf("f=%f, s='%s', ic=<none>\n", t, who);
		} else
			printf("f=%f, i=%d, s='%s'\n", t, ic, who);
	}
	printf("\nNow for the size tests...\n");

	real_uid = UID_MAX;
	(void) sprintf(buf, "%u", (unsigned int) real_uid);
	printf("buf(uid_t) = '%s'.\n", buf);

	real_uid = 0;
	(void) sscanf(buf, "%u", (unsigned int *) &real_uid);
	printf("real_uid = %u\n", real_uid);

	putchar('\n');

	usi = USHRT_MAX;
	(void) sprintf(buf, "%u", (unsigned short int) usi);
	printf("buf(usi) = '%s'.\n", buf);

	usi = 0;
	(void) sscanf(buf, "%hu", (unsigned short int *) &usi);
	printf("usi = %hu\n", (unsigned short int) usi);

	putchar('\n');

	ui = UINT_MAX;
	(void) sprintf(buf, "%u", (unsigned int) ui);
	printf("buf(ui) = '%s'.\n", buf);

	ui = 0;
	(void) sscanf(buf, "%u", (unsigned int *) &ui);
	printf("ui = %u\n", ui);

	putchar('\n');

	uli = ULONG_MAX;
	(void) sprintf(buf, "%lu", (unsigned long int) uli);
	printf("buf(uli) = '%s'.\n", buf);

	usi = 0;
	(void) sscanf(buf, "%lu", (unsigned long int *) &uli);
	printf("uli = %lu\n", uli);


	exit(0);
	/* NOTREACHED */
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " " fn " && echo 45.4 | ./" fn))
 * End:
 */
