/*
 *	getaddrinfo tester...
 *
 * compile with:
 *
 *	cc -g -O2 -o tgetaddrinfo tgetaddrinfo.c
 *
 * Copyright hereby placed in the Public Domain.
 */

#include <sys/types.h>
#include <sys/socket.h>
#ifdef HAVE_SYS_UN_H
# include <sys/un.h>
#endif
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define ADDR_T_STR(x)	(x == AF_INET ? "AF_INET" : \
			 (x == AF_INET6 ? "AF_INET6" : "<AF_UNKNOWN>"))

/*
 * FreeBSD (and Darwin in its image) is a bit brain-dead in the way they do
 * their multiple typedef avoidance -- i.e. they still follow the ancient
 * 4.4BSD style of using the fact that _BSD_SOCKLEN_T_ is NOT defined in order
 * to typedef socklen_t at the earliest point it's needed.  However they leave
 * no means for applications to know if the typedef has already been done.
 *
 * The most elegant way to protect typedefs is to prefix the type name with
 * "__" for the typedef and then use a CPP #define to map the true unprefixed
 * name to the actual typedef name after first providing a true typedef for the
 * unprefixed name for the compiler's internal use in aliasing the type.  This
 * way the presence of the type name as a #define tells us that the typedef for
 * it has already been done.
 *
 * All the other schemes are just inelegant hacks, but at least they're better
 * than having to know the details of individual OS library implementations!
 *
 * FYI: In NetBSD socklen_t came into use just before 1.3J:
 *
 *	(__NetBSD_Version__ - 0) > 103100000
 *
 * Not sure when GNU LibC added socklen_t, but it's in 2.1 at least.
 */
#if (defined(__FreeBSD__) || defined(__darwin__)) && defined(_BSD_SOCKLEN_T_)
# include "ERROR: something's wrong with the #includes above!"
#endif
/* Sigh, standards are such wonderful things.... */
#if !defined(socklen_t) && \
    !defined(__FreeBSD__) && !defined(__darwin__) && \
    !defined(_SOCKLEN_T) && !defined(__socklen_t_defined) && \
    (!defined(__GLIBC__) || (__GLIBC__ - 0) < 2) && \
    (!defined(__GLIBC_MINOR__) || (__GLIBC_MINOR__ - 0) < 1)
# if (/* SunOS-4 gcc */defined(__sun__) && !defined(__svr4__)) || \
     (/* SunOS-4 cc */defined(sun) && defined(unix) && !defined(__svr4__)) || \
     (/* 4.3BSD */defined(BSD) && ((BSD - 0) > 0) && ((BSD - 0) < 199506))
typedef int		__socklen_t;	/* 4.3BSD and older */
# else
typedef size_t		__socklen_t;	/* P1003.1g socket-related datum length */
# endif
typedef __socklen_t	socklen_t;
# define socklen_t	__socklen_t
#endif

#if defined(_SS_MAXSIZE)
# define HAVE_SOCKADDR_STORAGE
#endif

/*
 * sockaddrun_t -- a typedef for storage for any type of struct sockaddr*
 *
 * this union structure is like the "union res_sockaddr_union" from <resolv.h>,
 * or the "struct sockaddr_storage" from <sys/socket.h>, but:
 *
 * - it still includes a plain struct sockaddr field to allow generic sa_len
 *   and sa_family access
 *
 * - it doesn't need all the unnecessary padding because all the address types
 *   are directly accessible through their natural field names in their proper
 *   structs
 *
 * - it has nice(r) access macros to hide the union fields!
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
#ifdef HAVE_SYS_UN_H
	struct sockaddr_un saun_un;
# define saun_sun	saun_un
# define saun_sunpath	saun_un.sun_path
#endif
#ifdef HAVE_SOCKADDR_STORAGE
	struct sockaddr_storage saun_st;
#else
	int64_t		_saun_align;	/* probably max. necessary alignment */
#endif
} sockaddrun_t;

#define alignof(type)		(sizeof(struct {char d; type x;}) - sizeof(type))

#define print_alignof(type)	printf("alignment of %s: %ju\n", #type, (uintmax_t) alignof(type))

void check_alignof(const char *, size_t);

void
check_alignof(const char *ptr, size_t stride)
{
	if (((uintptr_t) ptr % (uintptr_t) stride) != 0) {
		printf("\nWARNING: un-aligned pointer at %p, (stride: %ju, off by %ju)\n",
		       ptr, stride,
		       (uintmax_t) ((uintptr_t) ptr % (uintptr_t) stride));
	}
	return;
}

static const char *streiaret(int, int);

static const char *
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

const char     *argv0 = "tgetaddrinfo";

int main(int, char *[]);

int
main(argc, argv)
	int argc;
	char *argv[];
{
	struct hostent *hp;
	struct addrinfo haip = {};	/* gcc "deep" initializer */
	struct addrinfo *aip;
	struct addrinfo *naip;
	int ret;

	argv0 = (argv0 = strrchr(argv[0], '/')) ? argv0 + 1 : argv[0];
	if (argc != 2) {
		fprintf(stderr, "Usage: %s name\n", argv0);
		exit(2);
	}

	print_alignof(char);
	print_alignof(char *);
	print_alignof(struct in_addr);
	print_alignof(struct sockaddr);
	print_alignof(struct sockaddr_in);
	print_alignof(struct sockaddr_in6);
#ifdef HAVE_SYS_UN_H
	print_alignof(struct sockaddr_un);
#endif
#ifdef HAVE_SOCKADDR_STORAGE
	print_alignof(struct sockaddr_storage);
#endif
	print_alignof(sockaddrun_t);

	if (!(hp = gethostbyname(argv[1]))) {
		fprintf(stderr, "%s: gethostbyname(%s): %s\n", argv0, (*argv[1] ? argv[1] : "''"), hstrerror(h_errno));
	} else {
		printf("gethostbyname()\n\tHostname:\t%s\n", hp->h_name);
		if (hp->h_aliases[0]) {
			printf("\tMore_Hostnames:\t");
		}
		while (hp->h_aliases[0]) {
			printf("%s ", *hp->h_aliases++);
		}
		if (hp->h_addr_list[0]) {
			printf("\n");
			printf("\tAddresses:\t");
		}
		while (hp->h_addr_list[0]) {
#if 1
			printf("%u.%u.%u.%u ",
			       (unsigned int) hp->h_addr_list[0][0],
			       (unsigned int) hp->h_addr_list[0][1],
			       (unsigned int) hp->h_addr_list[0][2],
			       (unsigned int) hp->h_addr_list[0][3]);
			hp->h_addr_list++;
#else
			struct sockaddr_in sin;

# ifdef DEBUG
			check_alignof((char *) (*(hp->h_addr_list)), alignof(struct in_addr));
# endif
			memcpy(&sin.sin_addr, *hp->h_addr_list++, (size_t) hp->h_length);
			printf("%s ", inet_ntoa(sin.sin_addr));
#endif
		}
		printf("\n\n");
	}

	haip.ai_family = PF_UNSPEC;		/* accept any protocol family */
	haip.ai_socktype = 0;			/* acccept any transport type */
#if 0
	haip.ai_flags = AI_PASSIVE;		/* null name == IN*ADDR_ANY */
#else
	haip.ai_flags = 0;			/* null name == loopback */
#endif
	haip.ai_flags |= AI_CANONNAME;		/* get canonical name */
	haip.ai_protocol = 0;			/* accept any protocol */
	if ((ret = getaddrinfo((*argv[1] ? argv[1] : (char *) NULL), (char *) NULL, &haip, &aip)) != 0) {
		int oerr = errno;

		fprintf(stderr,
			"%s: getaddrinfo(%s): %s\n",
			argv0,
			(*argv[1] ? argv[1] : "(NULL)"),
			streiaret(ret, oerr));
		exit(1);
		/* NOTREACHED */
	}

	printf("getaddrinfo(%s): Canonical Hostname: %s\nAddress(es)\n",
	       (*argv[1] ? argv[1] : "(NULL)"),
	       aip->ai_canonname);

	setprotoent(1);				/* minor efficiency tweak */

	for (naip = aip; naip; naip = naip->ai_next) {
		char abuf[NI_MAXHOST];
		const char *ccp;
		char *addr;		/* to allow for asprintf() */
		char *pname;
		sockaddrun_t *sun;
		struct protoent *ppe;

#if 0
		/*
		 * we could also use struct assignment to avoid all alignment
		 * issues with struct sockaddr vs sockaddrun_t
		 */
		sun.saun_sa = *naip->ai_addr;
#else
		sun = (sockaddrun_t *) (void *) naip->ai_addr;
		/* the cast through void * hides the alignment issue, so check */
		check_alignof((char *) sun, alignof(sockaddrun_t));
#endif
		if (naip->ai_family != sun->saun_family) {
			printf("\n*** ERROR IN NEXT ADDRINFO! -- AI_FAMILY(%s) != AI_ADDR->SA_FAMILY(%s)\n",
			       ADDR_T_STR(naip->ai_family),
			       ADDR_T_STR(sun->saun_family));
		}
		switch (naip->ai_family) {
		case AF_INET:
			if ((ccp = inet_ntop(naip->ai_family, &sun->saun_sinaddr, abuf, (socklen_t) sizeof(abuf)))) {
				addr = strdup(ccp);
			} else {
				asprintf(&addr, "(%s)", strerror(errno));
			}
                        break;
		case AF_INET6:
			if ((ccp = inet_ntop(naip->ai_family, &sun->saun_sin6addr, abuf, (socklen_t) sizeof(abuf)))) {
				addr = strdup(ccp);
			} else {
				asprintf(&addr, "(%s)", strerror(errno));
			}
                        break;
		default:
                        addr = strdup("<UNKNOWN> (ai_family != INET or INET6)");
                        break;
		}
		if ((ppe = getprotobynumber(naip->ai_protocol))) {
			pname = strdup(ppe->p_name);
		} else {
			asprintf(&pname, "(ai_protocol=%d)", naip->ai_protocol);
		}
		printf("\tip_proto = %s,\t%s =\t[%s]\n",
		       pname,
		       ADDR_T_STR(naip->ai_family),
		       addr);
		free(pname);
		free(addr);
	}

	freeaddrinfo(aip);

	exit(0);
	/* NOTREACHED */
}

/*
 * CPPFLAGS='-fstrict-aliasing -Wstrict-aliasing=1'
 *
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
