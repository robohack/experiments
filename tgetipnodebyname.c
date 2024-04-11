/*
 *	getipnodebyname() tester...
 *
 * compile with:
 *
 * cc -static -g -O2 -I/usr/pkg/bind/include -L/usr/pkg/lib -o tgetipnodebyname tgetipnodebyname.c -lbind
 *
 * Copyright hereby placed in the Public Domain.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define ADDR_T_STR(x)	(x == AF_INET ? "AF_INET" : \
			 (x == AF_INET6 ? "AF_INET6" : "UNKNOWN"))

const char     *argv0 = "tgetipnodebyname";

int main(int, char *[]);

int
main(argc, argv)
	int argc;
	char *argv[];
{
	struct hostent	*hep;
	unsigned int i;
	int my_h_errno;
	char buffer[BUFSIZ];			/* NI_MAXHOST */

	argv0 = (argv0 = strrchr(argv[0], '/')) ? argv0 + 1 : argv[0];
	if (argc != 2) {
		fprintf(stderr, "Usage: %s name|literal-address\n", argv0);
		exit(2);
	}
	if ((hep = getipnodebyname(argv[1], AF_INET, AI_DEFAULT, &my_h_errno))) {
		printf("getipnodebyname(AF_INET):\n");
		printf("\tHostname:\t%s\n", hep->h_name);
		printf("\tAliases:");
		for (i = 0 ; hep->h_aliases[i] ; i++)
			printf("\n\t\t%s", hep->h_aliases[i]);
		printf("\n");
		printf("\tAddr Type:\t%s\n", ADDR_T_STR(hep->h_addrtype));
		printf("\tAddr Length:\t%d\n", hep->h_length);
		printf("\tAddresses:");
		for (i = 0 ; hep->h_addr_list[i] != 0 ; i++) {
			if (! inet_ntop(hep->h_addrtype, hep->h_addr_list[i], buffer, (socklen_t) sizeof(buffer))) {
				fprintf(stderr, "\tinet_ntop(hp->h_addrtype = %d, h_addr_list[%d] = %d.%d.%d.%d): %s\n",
				        hp->h_addrtype,
				        i,
				        (unsigned int) hp->h_addr_list[0][0],
				        (unsigned int) hp->h_addr_list[0][1],
				        (unsigned int) hp->h_addr_list[0][2],
				        (unsigned int) hp->h_addr_list[0][3],
				        strerror(errno));
			} else {
				printf("\n\t\t%s", buffer);
			}
		}
		printf("\n\n");
		freehostent(hep);
	} else {
		fprintf(stderr, "%s: getipnodebyname(): %s\n", argv0, hstrerror(my_h_errno));
	}
	if ((hep = getipnodebyname(argv[1], AF_INET6, AI_ALL | AI_V4MAPPED, &my_h_errno))) {
		printf("getipnodebyname(AF_INET6):\n");
		printf("\tHostname:\t%s\n", hep->h_name);
		printf("\tAliases:");
		for (i = 0 ; hep->h_aliases[i] ; i++)
			printf("\n\t\t%s", hep->h_aliases[i]);
		printf("\n");
		printf("\tAddr Type:\t%s\n", ADDR_T_STR(hep->h_addrtype));
		printf("\tAddr Length:\t%d\n", hep->h_length);
		printf("\tAddresses:");
		for (i = 0 ; hep->h_addr_list[i] != 0 ; i++) {
			if (! inet_ntop(hep->h_addrtype, hep->h_addr_list[i], buffer, (socklen_t) sizeof(buffer))) {
				/* XXX the h_addr_list[] value only makes sense for AF_INET */
				fprintf(stderr, "\tinet_ntop(hp->h_addrtype = %d, h_addr_list[%d] = %d.%d.%d.%d): %s\n",
				        hp->h_addrtype,
				        i,
				        (unsigned int) hp->h_addr_list[0][0],
				        (unsigned int) hp->h_addr_list[0][1],
				        (unsigned int) hp->h_addr_list[0][2],
				        (unsigned int) hp->h_addr_list[0][3],
				        strerror(errno));
			} else {
				printf("\n\t\t%s", buffer);
			}
		}
		printf("\n\n");
		freehostent(hep);
	} else {
		fprintf(stderr, "%s: getipnodebyname(AF_INET6): %s\n", argv0, hstrerror(my_h_errno));
	}

	exit(0);
	/* NOTREACHED */
}

/*
 * cc -static -g -O2 -I/usr/pkg/bind/include -L/usr/pkg/lib -o tgetipnodebyname tgetipnodebyname.c -lbind
 *
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " LDLIBS=-lbind " fn " && ./" fn))
 * End:
 */
