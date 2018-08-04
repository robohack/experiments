#include <sys/cdefs.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <arpa/nameser.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <netdb.h>
#include <string.h>
#include <assert.h>

/*
 * N.B.:  gethostbyname() is unofficially deprecated by getaddrinfo()
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

#ifndef NS_MAXDNAME
# ifdef MAXDNAME
#  define NS_MAXDNAME	MAXDNAME
# else
#  error "NS_MAXDNAME is unknown"
# endif
#endif

/* use this as the "stride" parameter to check_alignof() */
#define alignof(type)   (sizeof(struct { char d; type x; }) - sizeof(type))

static bool check_alignof(const void *, size_t, const char *);

/*
 * N.B.:  stride should be given as "alignof(typeof(ptr))", but that's not
 * necessarily going to be strictly what the target architecture requires for
 * checking alignment of the object -- just what the compiler uses for optimum
 * alignment in structs.
 */
static bool
check_alignof(const void *ptr,
              size_t stride,
              const char *descr)
{
	if (((uintptr_t) ptr % (uintptr_t) stride) != 0) {
		printf("WARNING: un-aligned pointer to %s at %p, (stride: %ju, off by %ju)\n",
		       descr, ptr, (uintmax_t) stride,
		       (uintmax_t) ((uintptr_t) ptr % (uintptr_t) stride));
		return true;
#if 1 // verbose
	} else {
		printf("OK: alignment of pointer to %s at %p, (stride: %ju)\n",
		       descr, ptr, (uintmax_t) stride);
#endif
	}
	return false;
}


/*
 * Allocate enough memory to hold a fully-populated struct hostent
 *
 * The buffer required to storing all possible aliases and IP numbers accessed
 * by a struct hostent, according to Stevens' Unix Network Programming 2nd
 * edition, p. 304; is 8192 bytes, however OSF1 is known to require at least
 * 8872 bytes.
 */
enum {
	MAX_HOSTENT_BUF_SIZE = 9000,
};


/*
 * struct hostent fixers, adapted from curl-7.11.2 lib/hostip.c
 */
static struct hostent *pack_hostent(struct hostent *orig);

static void hostcache_fixoffset(struct hostent *h, long offset);

/*
 * pack_hostent() - perform a "deep" copy of a hostent into a newly allocated
 * buffer (returns a pointer to the copy).
 */
static struct hostent *
pack_hostent(struct hostent *orig)
{
	char *buf;
	char *bufptr;
	char *newbuf;
	struct hostent *copy;
	size_t i;
	char *str;
	size_t len;

	/* Make absolutely sure the destination buffer is big enough! */
	buf = bufptr = calloc((size_t) 1, (size_t) MAX_HOSTENT_BUF_SIZE);;
	if (buf == NULL) {
		return NULL;
	}
	copy = (struct hostent *) bufptr;

	bufptr += sizeof(struct hostent);
	copy->h_name = bufptr;
	len = strlen(orig->h_name) + 1;
	strncpy(bufptr, orig->h_name, len);
	bufptr += len;

	/* we align on even 64bit boundaries for safety */
#define MEMALIGN(x)	((x) + (8 - (((unsigned long) (x)) &0x7)))

	/* This must be aligned properly to work on many CPU architectures! */
	bufptr = MEMALIGN(bufptr);

	copy->h_aliases = (char **) bufptr;

	/* Figure out how many aliases there are */
	for (i = 0; orig->h_aliases && orig->h_aliases[i]; ++i) {
		;
	}

	/* Reserve room for the array */
	bufptr += (i + 1) * sizeof(char *);

	/* Clone all known aliases */
	if (orig->h_aliases) {
		for (i = 0; (str = orig->h_aliases[i]); i++) {
			len = strlen(str) + 1;
			strncpy(bufptr, str, len);
			copy->h_aliases[i] = bufptr;
			bufptr += len;
		}
	}
	/* if (!orig->h_aliases) then i was already set to 0 */

	/* Terminate the alias list with a NULL */
	copy->h_aliases[i] = NULL;

	copy->h_addrtype = orig->h_addrtype;
	copy->h_length = orig->h_length;

	/* align it for (at least) 32bit accesses */
	bufptr = MEMALIGN(bufptr);

	copy->h_addr_list = (char **) bufptr;

	/* Figure out how many addresses there are */
	for (i = 0; orig->h_addr_list[i] != NULL; ++i) {
		;
	}
	/* Reserve room for the array */
	bufptr += (i + 1) * sizeof(char *);

	i = 0;
	len = (size_t) orig->h_length;
	str = orig->h_addr_list[i];
	while (str != NULL) {
		memcpy(bufptr, str, len);
		copy->h_addr_list[i] = bufptr;
		bufptr += len;
		str = orig->h_addr_list[++i];
	}
	copy->h_addr_list[i] = NULL;
	copy = NULL;			/* prevent further access */

	/*
	 * now, shrink the allocated buffer to the size we actually need, which
	 * most often is only a fraction of the original alloc
	 */
	newbuf = realloc(buf, (size_t) (bufptr - buf));

	/* if the realloc() moved it, we need to adjust things again */
	if (newbuf != buf) {
		hostcache_fixoffset((struct hostent *) newbuf, (newbuf - buf));
	}

	return (struct hostent *) newbuf;
}

/*
 * hostcache_fixoffset() is a utility function that corrects all pointers in the
 * given hostent struct according to the offset.
 *
 * This is typically used when a hostent has been reallocated and needs to be
 * setup properly on the new address.
 */
static void
hostcache_fixoffset(struct hostent *h,
                    ptrdiff_t offset)
{
	int i = 0;

	h->h_name = (char *) ((long) h->h_name + offset);
	if (h->h_aliases) {
		/* only relocate aliases if there are any! */
		h->h_aliases = (char **) ((long) h->h_aliases + offset);
		while (h->h_aliases[i]) {
			h->h_aliases[i] = (char *) ((long) h->h_aliases[i] + offset);
			i++;
		}
	}

	h->h_addr_list = (char **) ((long) h->h_addr_list + offset);
	i=0;
	while(h->h_addr_list[i]) {
		h->h_addr_list[i] = (char *) ((long) h->h_addr_list[i] + offset);
		i++;
	}
}

static bool print_hostent(const struct hostent *hp);

static bool
print_hostent(const struct hostent *hp)
{
	unsigned int res = 0;
	unsigned int i;

	res += check_alignof(hp, alignof(struct hostent *), "hp");

	printf("Host: %s\n", hp->h_name);
	res += check_alignof(hp->h_aliases, alignof(char **), "hp->h_aliases");
	if (hp->h_aliases[0]) {
		printf("Other-Names:\n");
	}
	for (i = 0; hp->h_aliases[i] != NULL; i++) {
		/* xxx this alignment check is perhaps bogus? */
		res += check_alignof(hp->h_aliases[i], alignof(char *), "hp->h_aliases[i]");
		printf("\t%s\n", hp->h_aliases[i]);
	}
	printf("Addresses:\n");
	res += check_alignof(hp->h_addr_list, alignof(struct in_addr **), "hp->h_addr_list");
	for (i = 0 ; hp->h_addr_list[i] != 0 ; i++) {
		char buffer[BUFSIZ];		/* NI_MAXHOST */

		res += check_alignof(hp->h_addr_list[i], alignof(struct in_addr *), "hp->h_addr_list[i]");
		if (! inet_ntop(hp->h_addrtype, hp->h_addr_list[i], buffer, (socklen_t) sizeof(buffer))) {
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
			printf("\t%s\n", buffer);
		}
	}

	return res > 0;
}


int main(int, char *[]);

int
main(argc, argv)
	int argc;
	char *argv[];
{
	struct hostent *hp;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s HOSTNAME\n", argv[0]);
		exit(1);
	}
	if (strlen(argv[1]) > NS_MAXDNAME) {
		fprintf(stderr, "%s: parameter too long to be a proper hostname!\n", argv[0]);
		exit(1);
	}

	if (!(hp = gethostbyname(argv[1]))) {
		switch (h_errno) {
		case HOST_NOT_FOUND:
			printf("|%s| HOST_NOT_FOUND\n", argv[1]);
			break;
		case NO_RECOVERY:
			printf("|%s| NO_RECOVERY\n", argv[1]);
			break;
		case TRY_AGAIN:
			printf("|%s| TRY_AGAIN\n", argv[1]);
			break;
		case NO_DATA:
			printf("|%s| NO_DATA\n", argv[1]);
			break;
		}

		fprintf(stderr, "%s: gethostbyname(%s): %s\n", argv[0], argv[1], hstrerror(h_errno));
		exit(1);
	}
	printf("\nORIGINAL:\n\n");
	if (print_hostent(hp)) {
		struct hostent *nhp;

		nhp = pack_hostent(hp);
		if (nhp != NULL) {
			printf("\n\nCOPY:\n\n");
			if (print_hostent(nhp)) {
				printf("ERROR: in pack_hostent()!!!\n");
			}
			free(nhp);
		}
	}

	exit(0);
	/* NOTREACHED */
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
