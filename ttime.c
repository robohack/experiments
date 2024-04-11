#include <sys/cdefs.h>
#include <sys/param.h>
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#if (!defined(HAVE_INTTYPES_H) &&					\
     defined(__STDC_VERSION__) &&					\
      (__STDC_VERSION__ - 0) >= 199901L) ||				\
    defined(_ISOC99_SOURCE) ||						\
    (defined(__NetBSD_Version__) && (__NetBSD_Version__ >= 106000000)) || \
    (defined(__APPLE_CC__) && defined(__STDC__))
# define HAVE_INTTYPES_H	1
#endif

#ifdef HAVE_INTTYPES_H
# include <inttypes.h>
#endif

/*
 * XXX this is massively hackish -- best to use 
 */
#if (defined(__NetBSD_Version__) && (__NetBSD_Version__ >= 600000000)) || \
    (defined(__APPLE_CC__) && defined(__STDC__) && defined(__LP64__))
# define TIME_T_IS_64	/* defined */
# define TIME_T_MAX	LLONG_MAX
#else
# define TIME_T_MAX	LONG_MAX
#endif

#ifndef PRIdTIME
# if (TIME_T_MAX > INT32_MAX)
#  define INT_TIME_T	int64_t
#  define PRIdTIME	PRId64
# else
#  define INT_TIME_T	int32_t
#  define PRIdTIME	PRId32
# endif
#endif


int main(int, char *[]);

/* ARGSUSED */
int
main(argc, argv)
	int argc	__attribute__((unused));
	char *argv[]	__attribute__((unused));
{
	time_t t;
	int bits;

	t = 0;
	t = ~t;
	printf("\ntime_t is %s\n", t < 0 ? "signed" : "unsigned");

#ifdef __the_wrong_way__
	for (bits = 0, t = 1; t > 0; ++bits, t <<= 1) /* xxx evokes UB! */
		;
	bits++;
#else
	bits = sizeof(time_t) * CHAR_BIT;
#endif

#ifdef TIME_T_IS_64
	assert(sizeof(time_t) == sizeof(int64_t));
#else
	/* not long -- we don't know if long is int64_t or not */
	assert(sizeof(time_t) == sizeof(int32_t));
#endif
	printf("\ntime_t is %d bits\n", bits);

	t = time((time_t *) NULL);

	putchar('\n');

	printf("%s", ctime(&t));

	printf("long long:\t%lld\n", (long long) t);
	printf("u_longlong_t:\t%llu\n", (unsigned long long) t);


	t = (time_t) INT_MAX;
	printf("%s", ctime(&t));

	t = (time_t) LONG_MAX;
	printf("%s", ctime(&t));

#ifdef TIME_T_IS_64
	t = (time_t) LLONG_MAX;
	printf("%s", ctime(&t));
#else
	t = (time_t) LLONG_MAX;		/* time goes negative */
	printf("ovf: %s", ctime(&t));
#endif

	t = (time_t) -1;
	printf("%s", ctime(&t));

	t = (time_t) -1L;
	printf("%s", ctime(&t));

	t = (time_t) -1LL;
	printf("%s", ctime(&t));

	/* So, what's the best way to print the raw time_t scalar??? */

	printf("lld 1  %lld\n", (long long) ((time_t) 1));
	printf("lld -1 %lld\n", (long long) ((time_t) -1));
	printf("llu -1 %llu\n", (unsigned long long) ((time_t) -1));

	printf("%" PRIdTIME "\n", t); /* expects int, has long, for 32bit; expects long long int but has time_t on apple */
#ifdef TIME_T_IS_64
	printf("%" PRIdTIME "\n", (int64_t) t); /* OK, for 64bit */
#else
	printf("%" PRIdTIME "\n", (int32_t) t); /* OK, for 32bit */ /* ttime.c:95: warning: format '%ld' expects type 'long int', but argument 2 has type 'time_t' */
#endif
#ifdef TIME_T_IS_64
	printf("%" PRIdTIME "\n", (int64_t) INT64_MAX); /* OK, for 64bit */
#else
	printf("%" PRIdTIME "\n", (int32_t) INT32_MAX); /* OK, for 32bit */ /* ttime.c:100: warning: format '%ld' expects type 'long int', but argument 2 has type 'int' */
#endif
	printf("%" PRIdTIME "\n", (INT_TIME_T) t); /* OK!!!! */

#ifdef HAVE_INTTYPES_H
	puts("\nJust use intmax!");
	printf("%jd\n", (intmax_t) t); /* OK!!!! */
	printf("%ju\n", (uintmax_t) t); /* OK!!!! */
#endif

	exit(0);
	/* NOTREACHED */
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
