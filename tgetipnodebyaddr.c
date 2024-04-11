/*
 *	getipnodebyaddr() tester...
 *
 * compile with:
 *
 * cc -static -g -O2 -I/usr/pkg/bind/include -L/usr/pkg/lib -o tgetipnodebyaddr tgetipnodebyaddr.c -lbind
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

/*
 * sockaddrun_t -- a typedef for storage for any type of struct sockaddr*
 *
 * this union structure is like the "union res_sockaddr_union" from <resolv.h>,
 * but:
 *
 * - it includes a plain struct sockaddr field to allow generic sa_len and
 *   sa_family access
 *
 * - it doesn't include the unnecessary __align and __space padding.
 *
 * - it has nice access macros to hide the union fields!
 */
typedef union sockaddr_union {
		struct sockaddr saun_sa;
#define saun_len	saun_sa.sa_len
#define saun_family	saun_sa.sa_family
		struct sockaddr_in saun_sa4;
#define saun_sin	saun_sa4
#define saun_sinaddr	saun_sa4.sin_addr
#ifdef AF_INET6
		struct sockaddr_in6 saun_sa6;
# define saun_sin6	saun_sa6
# define saun_sin6addr	saun_sa6.sin6_addr
#endif
} sockaddrun_t;

const char     *argv0 = "tgetipnodebyaddr";

int main(int, char *[]);

int
main(argc, argv)
	int argc;
	char *argv[];
{
	sockaddrun_t saun;
	struct hostent	*hep;
	int ret;
	unsigned int i;
	int my_h_errno;
	char buffer[BUFSIZ];			/* NI_MAXHOST */
	void *addr_p;
	size_t addr_sz;

	argv0 = (argv0 = strrchr(argv[0], '/')) ? argv0 + 1 : argv[0];
	if (argc != 2) {
		fprintf(stderr, "Usage: %s address\n", argv0);
		exit(2);
	}
#ifdef AF_INET6
	if (strchr(argv[1], ':')) {
		saun.saun_len = sizeof(saun.saun_sin6);
		saun.saun_family = AF_INET6;
		addr_p = &(saun.saun_sin6addr);
		addr_sz = sizeof(saun.saun_sin6addr);
		ret = inet_pton(saun.saun_family, argv[1], addr_p);
	} else
#endif
	{
		saun.saun_len = sizeof(saun.saun_sin);
		saun.saun_family = AF_INET;
		addr_p = &(saun.saun_sinaddr);
		addr_sz = sizeof(saun.saun_sinaddr);
		ret = inet_pton(saun.saun_family, argv[1], addr_p);
	}
	if (ret != 1) {
		fprintf(stderr, "%s: error with '%s': %s\n", argv0, argv[1],
			(ret == 0) ? "unparsable address" : strerror(errno));
		exit(1);
	}
	if ((hep = getipnodebyaddr(addr_p, addr_sz, saun.saun_family, &my_h_errno))) {
		printf("getipnodebyaddr():\n");
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
		fprintf(stderr, "%s: getipnodebyaddr(): %s\n", argv0, hstrerror(my_h_errno));
	}

	exit(0);
	/* NOTREACHED */
}

/*

 * cc -static -g -O2 -I/usr/pkg/bind/include -L/usr/pkg/lib -o tgetipnodebyaddr tgetipnodebyaddr.c -lbind

 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " LDLIBS=-lbind " fn " && ./" fn))
 * End:
 */
