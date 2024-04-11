/*
 *	getnameinfo tester...
 *
 * getnameinfo() is almost completely useless.  See getipnodebyaddr().
 *
 * compile with:
 *
 *	cc -g -O2 -o tgetnameinfo tgetnameinfo.c
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
#define saun_data	saun_sa.sa_data
		struct sockaddr_in saun_sa4;
#define saun_sin	saun_sa4
#define saun_sinaddr	saun_sa4.sin_addr
#ifdef AF_INET6
		struct sockaddr_in6 saun_sa6;
# define saun_sin6	saun_sa6
# define saun_sin6addr	saun_sa6.sin6_addr
#endif
} sockaddrun_t;

const char *streiaret(int, int);

const char *
streiaret(eia, err)
	int eia;
	int err;
{
	switch (eia) {
	case EAI_SYSTEM:
		return strerror(err);
	default:
		return gai_strerror(eia);
	}
}

const char     *argv0 = "tgetnameinfo";

int main(int, char *[]);

int
main(argc, argv)
	int argc;
	char *argv[];
{
	sockaddrun_t saun;
	char hbuf[NI_MAXHOST];			/* output */
	struct hostent *hp;
	int ret;
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
		fprintf(stderr, "%s: error with '%s': %s\n", argv0, argv[1], (ret == 0) ? "unparsable address" : strerror(errno));
		exit(1);
	}
	if (saun.saun_family == AF_INET) {
		if ((hp = gethostbyaddr(addr_p, (socklen_t) addr_sz, AF_INET))) {
			unsigned int i;

			printf("gethostbyaddr()\n\tHostname:\t%s\n", hp->h_name);
			printf("\tAliases:\t");
			while (hp->h_aliases[0])
				printf("%s ", *hp->h_aliases++);
			printf("\n");
			printf("\tAddresses:\t");
			for (i = 0 ; hp->h_addr_list[i] != NULL ; i++) {
				char buffer[BUFSIZ];		/* NI_MAXHOST */

				if (! inet_ntop(hp->h_addrtype, hp->h_addr_list[i], buffer, (socklen_t) sizeof(buffer))) {
					fprintf(stderr, "\tinet_ntop(hp->h_addrtype = %d, h_addr_list[%d] = %d.%d.%d.%d): %s\n",
					        hp->h_addrtype,
					        i,
					        (unsigned int) hp->h_addr_list[0][0],
					        (unsigned int) hp->h_addr_list[0][1],
					        (unsigned int) hp->h_addr_list[0][2],
					        (unsigned int) hp->h_addr_list[0][3],
					        strerror(errno));
				} else {
					printf("%s ", buffer);
				}
			}
			printf("\n\n");
		} else {
			fprintf(stderr, "%s: gethostbyaddr(): %s\n", argv0, hstrerror(h_errno));
		}
	} else {
		printf("gethostbyaddr(): no support for address type %s", ADDR_T_STR(saun.saun_family));
	}

	if ((ret = getnameinfo(&saun.saun_sa, saun.saun_len, hbuf, (socklen_t) sizeof(hbuf), (char *) NULL, 0, NI_NAMEREQD)) != 0) {
		int oerr = errno;
		struct sockaddr *sap = &saun.saun_sa;
		char abuf[NI_MAXHOST];
		const char *addr;

#ifdef AF_INET6
		if (sap->sa_family == AF_INET6) {
			addr = inet_ntop(AF_INET6, &saun.saun_sin6addr, abuf, (socklen_t) sizeof(abuf));
		} else
#endif
		{
			addr = inet_ntop(AF_INET, &saun.saun_sinaddr, abuf, (socklen_t) sizeof(abuf));
		}
		fprintf(stderr,
			"%s: getnameinfo(%s[%s], %d): %s\n",
			argv0,
			ADDR_T_STR(sap->sa_family),
		        addr ? addr : strerror(errno),
			sap->sa_len,
			streiaret(ret, oerr));
		exit(1);
		/* NOTREACHED */
	}
	printf("getnameinfo()\n\tHostname:\t%s\n", hbuf);

	exit(0);
	/* NOTREACHED */
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
