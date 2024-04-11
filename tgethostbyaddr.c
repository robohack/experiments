#include <sys/cdefs.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <netdb.h>

/*
 * N.B.:  gethostbyaddr() is unofficially deprecated by getnameinfo()
 */

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

/*
 * sockaddrun_t -- a typedef for storage for any type of struct sockaddr*
 *
 * this union structure is like the "union res_sockaddr_union" from <resolv.h>,
 * or the "struct sockaddr_storage" from <sys/socket.h>, but:
 *
 * - it includes a plain struct sockaddr field to allow generic sa_len and
 *   sa_family access
 *
 * - it doesn't include the unnecessary (for normal C use) __align and __space
 *   padding.
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
#ifdef HAVE_SYS_UN_H
	struct sockaddr_un saun_un;
# define saun_unpath	saun_un.sun_path
#endif
} sockaddrun_t;

/*
 * the rest of this goop is just for testing some stuff from host(1)....
 */
#if !defined(in_addr_t)
/* note: assume the preprocessor is using at least 32-bit unsigned long */
# if UINT_MAX == 0xFFFFFFFFU
typedef unsigned int	ipaddr_t;
# elif ULONG_MAX == 0xFFFFFFFFU
typedef unsigned long	ipaddr_t;
# else
#  include "houston, we have a problem!"
# endif
#else
typedef in_addr_t	ipaddr_t;
#endif

#define alignof(type)   (sizeof(struct { char d; type x; }) - sizeof(type))

void check_alignof(const void *, size_t, const char *);

/*
 * N.B.:  stride should be given as "alignof(ptr)", but that's not necessarily going
 * to be strictly what the target architecture requires for checking alignment
 * of the object -- just what the compiler uses for optimum alignment in structs.
 */
void
check_alignof(const void *ptr,
              size_t stride,
              const char *descr)
{
	if (((uintptr_t) ptr % (uintptr_t) stride) != 0) {
		printf("WARNING: un-aligned pointer to %s at %p, (stride: %ju, off by %ju)\n",
		       descr, ptr, stride,
		       (uintmax_t) ((uintptr_t) ptr % (uintptr_t) stride));
	}
	return;
}

#define NOT_DOTTED_QUAD	((ipaddr_t) -1)
#define BROADCAST_ADDR	((ipaddr_t) 0xffffffffU)
#define LOCALHOST_ADDR	((ipaddr_t) 0x7f000001U)

int main(int, char *[]);

int
main(
	int argc,
	char *argv[])
{
	struct sockaddr_in from_sa;
#if 0
	socklen_t from_sa_len = sizeof(from_sa);
#endif
	struct hostent *hp;
	unsigned int i;
	uint32_t inet;
	char buffer[BUFSIZ];		/* NI_MAXHOST */

	assert(INADDR_NONE == NOT_DOTTED_QUAD);
	assert(sizeof(ipaddr_t) == 4);

	if (argc != 2) {
		fprintf(stderr, "Usage: %s INET_ADDR\n", argv[0]);
		exit(1);
	}
	if ((inet = inet_addr(argv[1])) == INADDR_NONE) {
		fprintf(stderr, "%s: '%s' is not a valid INET address number\n", argv[0], argv[1]);
	}

	from_sa.sin_family = AF_INET;	/* XXX code here still only handles AF_INET */
	from_sa.sin_port = htons((uint16_t) (in_port_t) 65535); /* xxx can't win against gcc here */
	from_sa.sin_addr.s_addr = inet;

	if (! inet_ntop(from_sa.sin_family, &(from_sa.sin_addr), buffer, (socklen_t) sizeof(buffer))) {
		fprintf(stderr, "%s: inet_ntop((void *) &(from_sa.sin_addr)): %s\n", argv[0], strerror(errno));
		exit(1);
	}

	if (!(hp = gethostbyaddr((void *) &(from_sa.sin_addr),
	                         (socklen_t) sizeof(from_sa.sin_addr), from_sa.sin_family))) {
		herror(argv[1]);
		switch (h_errno) {
		case HOST_NOT_FOUND:
			printf("|%s| HOST_NOT_FOUND\n", buffer);
			break;
		case NO_RECOVERY:
			printf("|%s| NO_RECOVERY\n", buffer);
			break;
		case TRY_AGAIN:
			printf("|%s| TRY_AGAIN\n", buffer);
			break;
		case NO_DATA:
			printf("|%s| NO_DATA\n", buffer);
			break;
		}

		fprintf(stderr, "%s: gethostbyaddr(%s): %s\n", argv[0], argv[1], hstrerror(h_errno));
		exit(1);
	}
	check_alignof(hp, alignof(struct hostent *), "hp");
	printf("Host: %s\n", hp->h_name);
	if (hp->h_aliases[0]) {
		printf("Other-Names:\n");
	}
	for (i = 0; hp->h_aliases[i] != NULL; i++) {
		/* xxx this alignment check is perhaps bogus? */
		check_alignof(hp->h_aliases[i], alignof(char *), "hp->h_aliases[i]");
		printf("\t%s\n", hp->h_aliases[i]);
	}
	printf("Addresses:\n");
	for (i = 0 ; hp->h_addr_list[i] != NULL ; i++) {
		check_alignof(hp->h_addr_list[i], alignof(struct in_addr *), "hp->h_addr_list[i]");
		/* xxx should the memcpy trick be used here too? */
		if (! inet_ntop(hp->h_addrtype, hp->h_addr_list[i], buffer, (socklen_t) sizeof(buffer))) {
#if 0
			/* XXX the h_addr_list[] value only makes sense for AF_INET */
			fprintf(stderr, "\tinet_ntop(hp->h_addrtype = %d, h_addr_list[%d] = %d.%d.%d.%d): %s\n",
				hp->h_addrtype,
				i,
			        (unsigned int) hp->h_addr_list[0][0],
			        (unsigned int) hp->h_addr_list[0][1],
			        (unsigned int) hp->h_addr_list[0][2],
			        (unsigned int) hp->h_addr_list[0][3],
				strerror(errno));
#else
			struct sockaddr_in sin;

# ifdef DEBUG
			check_alignof((char *) (*(hp->h_addr_list)), alignof(struct in_addr));
# endif
			memcpy(&sin.sin_addr, *hp->h_addr_list++, (size_t) hp->h_length);
			printf("%s ", inet_ntoa(sin.sin_addr));
#endif
		} else {
			printf("\t%s\n", buffer);
		}
	}

	exit(0);
	/* NOTREACHED */
}

/*
 * CPPFLAGS='-fstrict-aliasing -Wstrict-aliasing=1'
 *
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
