#define HAVE_SYS_SYSLIMITS_H
#ifdef _SVR4_SOURCE
# undef HAVE_SYS_SYSLIMITS_H
#endif

#include <sys/types.h>
#ifdef HAVE_SYS_SYSLIMITS_H
# include <sys/syslimits.h>
#endif
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#if !defined(HAVE_QUAD_T) && !defined(quad_t)
typedef int64_t         quad_t;
#endif
#if !defined(HAVE_U_QUAD_T) && !defined(u_quad_t)
typedef uint64_t      u_quad_t;
#endif

int main(int, char *[]);

/*
0123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 12
 */
int
main(argc, argv)
	int argc;
	char *argv[];
{
	uid_t user_id = 65534;
	int x;

	(void) argc;

	printf("sizeof(uid_t) =\t\t%lu\n", (unsigned long) sizeof(uid_t));
	printf("sizeof(int) =\t\t%lu\n", (unsigned long) sizeof(int));
	printf("sizeof(short) =\t\t%lu\n", (unsigned long) sizeof(short));
	printf("sizeof(long) =\t\t%lu\n", (unsigned long) sizeof(long));
	printf("sizeof(quad_t) =\t%lu\n", (unsigned long) sizeof(quad_t));
	printf("sizeof(int64_t) =\t%lu\n", (unsigned long) sizeof(int64_t));
	printf("sizeof(long long) =\t%lu\n", (unsigned long) sizeof(long long));
	printf("sizeof(float) = \t%lu\n", (unsigned long) sizeof(float));
	printf("sizeof(double ) =\t%lu\n", (unsigned long) sizeof(double));
	printf("sizeof(long double) =\t%lu\n", (unsigned long) sizeof(long double));
	printf("sizeof(void *) =\t%lu\n", (unsigned long) sizeof(void *));

	printf("uid_t user_id = 65534 = (int) %d, (unsigned int) %u, (long) %ld, (unsigned long) %lu\n",
	       (int) user_id,
	       (unsigned int) user_id,
	       (long) user_id,
	       (unsigned long) user_id);

	user_id = -1;			/* warning: negative integer implicitly converted to unsigned type */
	printf("uid_t user_id = -1 = (int) %d, (unsigned int) %u, (long) %ld, (unsigned long) %lu\n",
	       (int) user_id,
	       (unsigned int) user_id,
	       (long) user_id,
	       (unsigned long) user_id);

	user_id = -2;			/* warning: negative integer implicitly converted to unsigned type */
	printf("uid_t user_id = -2 = (int) %d, (unsigned int) %u, (long) %ld, (unsigned long) %lu\n",
	       (int) user_id,
	       (unsigned int) user_id,
	       (long) user_id,
	       (unsigned long) user_id);

	user_id = (uid_t) 4294967296U;
	printf("uid_t user_id = 2^32 = (int) %d, (unsigned int) %u, (long) %ld, (unsigned long) %lu\n",
	       (int) user_id,
	       (unsigned int) user_id,
	       (long) user_id,
	       (unsigned long) user_id);

	printf("(uid_t) -1 = %d, %u\n", (uid_t) -1, (uid_t) -1);
	printf("(short) -1 = %hd, %hu\n", (short) -1, (unsigned short) -1);
	printf("(int) -1 = %d, %u\n", (int) -1, (unsigned int) -1);
	printf("(long) -1 = %ld, %lu\n", (long) -1, (unsigned long) -1);
#ifdef HAVE_PCT_Q
	printf("(quad_t) -1 = %qd, %qu\n", (quad_t) -1, (u_quad_t) -1);
#endif
	printf("(long long) -1 = %lld, %llu\n", (long long) -1, (unsigned long long) -1);

	printf("(uid_t) -2 = %d, %u\n", (uid_t) -2, (uid_t) -2);
	printf("(short) -2 = %hd, %hu\n", (short) -2, (unsigned short) -2);
	printf("(int) -2 = %d, %u\n", (int) -2, (unsigned int) -2);
	printf("(long) -2 = %ld, %lu\n", (long) -2, (unsigned long) -2);
#ifdef HAVE_PCT_Q
	printf("(quad_t) -2 = %qd, %qu\n", (quad_t) -2, (u_quad_t) -2);
#endif
	printf("(long long) -2 = %lld, %llu\n", (long long) -2, (unsigned long long) -2);

	printf("(uid_t) UID_MAX = %d, %u\n", (uid_t) UID_MAX, (uid_t) UID_MAX);
	printf("(short) UID_MAX = %hd, %hu\n", (short) UID_MAX, (unsigned short) UID_MAX);
	printf("(int) UID_MAX = %d, %u\n", (int) UID_MAX, (unsigned int) UID_MAX);
	printf("(long) UID_MAX = %ld, %lu\n", (long) UID_MAX, (unsigned long) UID_MAX);
#ifdef HAVE_PCT_Q
	printf("(quad_t) UID_MAX = %qd, %qu\n", (quad_t) UID_MAX, (u_quad_t) UID_MAX);
#endif
	printf("(long long) UID_MAX = %lld, %llu\n", (long long) UID_MAX, (unsigned long long) UID_MAX);

	printf("(uid_t) UINT_MAX = %d, %u\n", (uid_t) UINT_MAX, (uid_t) UINT_MAX);
	printf("(uid_t) UINT_MAX-1 = %d, %u\n", (uid_t) UINT_MAX-1, (uid_t) UINT_MAX-1);
	printf("(uid_t) UINT_MAX-2 = %d, %u\n", (uid_t) UINT_MAX-2, (uid_t) UINT_MAX-2);

	printf("(uid_t) (~(uid_t) 0) = %d, %u\n", (uid_t) (~(uid_t) 0), (uid_t) (~(uid_t) 0));
	printf("(uid_t) (~(uid_t) 0 - 1) = %d, %u\n", (uid_t) (~(uid_t) 0 - 1), (uid_t) (~(uid_t) 0 - 1));
	printf("(uid_t) (~(uid_t) 0 - 2) = %d, %u\n", (uid_t) (~(uid_t) 0 - 2), (uid_t) (~(uid_t) 0 - 2));
	printf("(uid_t) ((~(uid_t) 0) - 2) = %d, %u\n", (uid_t) ((~(uid_t) 0) - 2), (uid_t) ((~(uid_t) 0) - 2));

	if ((x = setuid(user_id)) < 0) {
		fprintf(stderr, "%s: setuid(%d) = %d: ", argv[0], user_id, x);
		perror("failed");
	}
	printf("getuid() = %d, ", getuid());
	printf("geteuid() = %d\n", geteuid());

	exit(0);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
