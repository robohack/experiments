#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/nameser.h>
#include <resolv.h>

#include <netdb.h>

extern int h_errno;
char errbuf[200];

int main(int, char*[]);

int
main(argc, argv)
	int argc;
	char *argv[];
{
	char host[MAXDNAME + 1];
	struct hostent *hep;
	int error = 0;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s somehost\n", argv[0]);
		exit(1);
	}

	if (strlen(argv[1]) > sizeof(host)-1) {
		fprintf(stderr, "%s: parameter too long to be a proper hostname!\n", argv[0]);
		exit(1);
	}

	strncpy(host, argv[1], sizeof(host));
	host[sizeof(host) - 1] = '\0';

	if (! (hep = gethostbyname(host))) {
		switch (h_errno) {
		case HOST_NOT_FOUND:
			printf("|%s| HOST_NOT_FOUND\n", host);
			error = 1;
			break;
		case NO_RECOVERY:
			printf("|%s| NO_RECOVERY\n", host);
			error = 1;
			break;
		case TRY_AGAIN:
			printf("|%s| TRY_AGAIN\n", host);
			error = 1;
			break;
		case NO_DATA:
			printf("|%s| NO_DATA\n", host);
			error = 0;
			break;
		}
	}

	printf("%s: %s\n", host, !hep ? "no such host" : error ? "is bad" : "is good");
	if (error)
		printf("hstrerror(%d) returns `%s'\n", h_errno, hstrerror(h_errno));

	exit(0);
	/* NOTREACHED */
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
