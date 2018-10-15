#if (!defined(HAVE_SYS_CDEFS_H) &&                                      \
     (defined(__linux__) ||						\
      defined(BSD) ||							\
      defined(__NetBSD__) ||                                            \
      defined(__FreeBSD__) ||                                           \
      defined(__OpenBSD__) ||                                           \
      defined(__Darwin__) ||                                            \
      defined(__DragonFly__) ||                                         \
      defined(__APPLE__)))
# define HAVE_SYS_CDEFS_H		/* defined */
#endif

#if defined(HAVE_SYS_CDEFS_H)
# include <sys/cdefs.h>
#endif

#if (!defined(HAVE_SYS_PARAM_H) &&                                      \
     (defined(BSD) ||                                                   \
      defined(__NetBSD__) ||                                            \
      defined(__FreeBSD__) ||                                           \
      defined(__OpenBSD__) ||                                           \
      defined(__Darwin__) ||                                            \
      defined(__DragonFly__) ||                                         \
      defined(__APPLE__)))
# define HAVE_SYS_PARAM_H		/* defined */
#endif

#if defined(HAVE_SYS_PARAM_H)
# include <sys/param.h>
#endif

/*
 * also avoiding requiring external tests for <stdbool.h>
 */
#if (!defined(HAVE_STDBOOL_H) &&                                        \
     ((defined(__STDC_VERSION__) &&                                     \
       ((__STDC_VERSION__ - 0) >= 199901L)) ||                          \
      defined(_ISOC99_SOURCE) ||                                        \
      (defined(__NetBSD_Version__) &&                                   \
       ((__NetBSD_Version__ - 0) >= 106000000)) ||                      \
      (defined(__APPLE_CC__) && defined(__STDC__)) ||                   \
      defined(__linux__)))
# define HAVE_STDBOOL_H
# define HAVE_SYS_TYPES_H
# define HAVE_WCHAR_H			/* ??? */
#endif

#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif

#ifdef HAVE_STDBOOL_H
# include <stdbool.h>
#else /* !HAVE_STDBOOL_H  */
typedef enum bool_e { false = 0U, true = !false } bool;
#endif /* !HAVE_STDBOOL_H  */

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_WCHAR_H
# include <wchar.h>
#endif

/*
 * If we know CHAR_BIT, we know everything!
 *
 * Well, sort of....  It is possible for an implementation to not use all of
 * the bits in any scalar type other than "unsigned char".
 *
 * I.e.:  the possible range of values that an integer type can represent shall
 * not be calculated from the number of bits in its object representation.
 *
 * So for true portability one must use the macros in <limits.h>.
 */

/* N.B.:  None of these macros can be used for preprocessor expressions! */

/* xxx expect "warning: will never be executed" for almost all puts() calls */

/*
 * works for all unsigned types (assuming "ULL" is the modifier for the largest
 * possible literal unsigned int)
 */
#define MAXUINT(T) ((T) ~0ULL)

/*
 * Obviously this one only works for signed types, and only on 2's-complement
 *
 * for "char" as T, this also only works if "char" is naturally "signed char"
 * (for naturally "unsigned char" it'll be half as big as it should be)
 *
 * This works by creating an all-1-bits unsigned representation of the type,
 * then shifting in a zero bit (as is required by the C standard for right
 * shifting on an unsigned type), and finally converting it back to its signed
 * representation.
 */
#define MAXSINT(T) ((T) (((unsigned T) ~0ULL) >> 1))

#define MAXSINTx(T) ((T) (((T) ~0ULL) >> 1))


/*
 * Obviously there is no use for MIN calculations except for signed tupes
 */
/* xxx this won't work with an explicit "signed char" */
#define MINSINT(T) (-((T) (((unsigned T) ~0ULL) >> 1)) - 1)

/* xxx this won't work with just "char" for naturally unsigned char -- the result is positive!!! */
#define MINSINTx(T) ((T) (((T) 1ULL << ((sizeof(T) * CHAR_BIT) - 1U))))

/*
 * This only works for signed types too, however it can be given an unsigned
 * type and so it can be used as a helper in the universal MAX_INT() below.
 *
 * For unsigned types the result will naturally be half as big as it should be.
 *
 * Note the careful use of unsigned constants.  This is done because shifting a
 * 1 into the sign bit position of a signed value, is not "allowed".  Even if
 * your machine do this, the resulting value, a 1 followed by all 0's is not
 * necessarily a valid value for a signed type, but may actually be a trap
 * representation.  In most two's complement architectures it is the min value,
 * but it doesn't have to be.
 *
 * So instead we turn on the maximum (left-most) one bit for the type, then do
 * an XOR with the value of -1 for the type.  (A cast to a smaller signed value
 * must preserve the sign bit.)
 *
 * Note the cast of the shifted value avoids a "warning: negative integer
 * implicitly converted to unsigned type"
 */
#define MAXINT(T) ((T) ((T) (1ULL << ((sizeof(T) * CHAR_BIT) - 1U)) ^ ((T) -1LL)))

#if 0
/*
 * By checking if (T)-1 is actually less than zero we can decide which of
 * MAXINT() or MAXUINT() to use.
 *
 * Unfortunately because the comparison of (unsigned)-1 < 0 is always false
 * some compilers will warn about this obvious fact at every expansion of the
 * macro, just to annoy us.
 */
# define MAX_INT(T)			\
	(((T) -1 < 0) ? MAXINT(T)	\
	              : MAXUINT(T))
#else
/*
 * by doing what appears to be slightly more work we can avoid all warnings
 * (however on at least amd64 this creates the same object code)
 */
# define MAX_INT(T) ((MAXINT(T) == (MAXINT(T) | MAXUINT(T) >> 1)) ? MAXUINT(T) \
                                                                  : MAXINT(T))
#endif

#define MAX_OF(type) \
    (((type) ~0LLU > (type) ((1LLU<<((sizeof(type)<<3)-1))-1LLU)) ? (long long unsigned int) (type) ~0LLU : \
                                                                    (long long unsigned int) (type) ((1LLU << ((sizeof(type) << 3) - 1)) - 1LLU))

#define MIN_OF(type) \
    (((type) (1LLU << ((sizeof(type) << 3) - 1)) < (type) 1) ? (long long int) (~0LLU - ((1LLU << ((sizeof(type) << 3) - 1)) - 1LLU)) : \
                                                               0LL)

int main(void);

int
main()
{
	volatile unsigned int bits;
	volatile unsigned int char_bits;

	char_bits = 0;
	bits = 1;
	while ((~0U & (~0U >> (bits - 1))) != 1) {
		bits++;
	}
	if (bits % sizeof(int) == 0) {
		char_bits = bits / sizeof(int);
	}
	printf("char bits = %d %s CHAR_BIT\n", char_bits,
	       char_bits == CHAR_BIT ? "==" : "!=");

	if (sizeof(char) != 1) {
		printf("WARNING:   sizeof(char) is not '1'!!! (%lld != %lld)\n",
		       (long long) sizeof(char), (long long) 1);
	}
	if (MAX_INT(char) != CHAR_MAX) {
		printf("NOTICE:   MAX_INT() suggests char is unsigned !!! (%lld != %lld)\n",
		       (long long) MAX_INT(char), (long long) CHAR_MAX);
	}
	if (MINSINTx(char) != CHAR_MIN) {
		printf("WARNING:  MINSINTx() says char is unsigned !!! (%lld != %lld)\n",
		       (long long) MINSINTx(char), (long long) CHAR_MIN);
	}
	if (((char) (((char) 1ULL << ((sizeof(char) * CHAR_BIT) - 1U)))) != CHAR_MIN) {
		printf("WARNING:  char really is unsigned !!! (%lld != %lld)\n",
		       (long long) ((char) (((char) 1ULL << ((sizeof(char) * CHAR_BIT) - 1U)))),
		       (long long) CHAR_MIN);
	}

#ifdef WEOF
	if (MAX_INT(wchar_t) != WCHAR_MAX) {
		printf("NOTICE:   MAX_INT() suggests wchar_t is unsigned !!! (%lld != %lld)\n",
		       (long long) MAX_INT(wchar_t), (long long) WCHAR_MAX);
	}
	if (MINSINTx(wchar_t) != WCHAR_MIN) {
		printf("WARNING:  MINSINTx() says wchar_t is unsigned !!! (%lld != %lld)\n",
		       (long long) MINSINTx(wchar_t), (long long) WCHAR_MIN);
	}
#endif

	/* proof */
	printf("MAXUINT(ulong long)  = 0x%18llx, ULLONG_MAX = 0x%18llx\n",
	       (unsigned long long) MAXUINT(unsigned long long),
	       (unsigned long long) ULLONG_MAX);
	if (MAXUINT(unsigned long long) != ULLONG_MAX) {
		puts("WARNING:  UINTMAX(unsigned long long) != ULLONG_MAX !!!");
	}
	printf("MAXUINT(ulong)       = 0x%18llx, ULONG_MAX =  0x%18llx\n",
	       (unsigned long long) MAXUINT(unsigned long),
	       (unsigned long long) ULONG_MAX);
	if (MAXUINT(unsigned long) != ULONG_MAX) {
		puts("WARNING:  UINTMAX(unsigned long) != ULONG_MAX !!!");
	}
	printf("MAXUINT(uint)        = 0x%18llx, UINT_MAX =   0x%18llx\n",
	       (unsigned long long) MAXUINT(unsigned int),
	       (unsigned long long) UINT_MAX);
	if (MAXUINT(unsigned int) != UINT_MAX) {
		puts("WARNING:  UINTMAX(unsigned int) != UINT_MAX !!!");
	}
	printf("MAXUINT(ushort)      = 0x%18llx, USHRT_MAX  = 0x%18llx\n",
	       (unsigned long long) MAXUINT(unsigned short),
	       (unsigned long long) USHRT_MAX);
	if (MAXUINT(unsigned short) != USHRT_MAX) {
		puts("WARNING:  UINTMAX(unsigned short) != USHRT_MAX !!!");
	}
	printf("MAXUINT(uchar)       = 0x%18llx, UCHAR_MAX  = 0x%18llx\n",
	       (unsigned long long) MAXUINT(unsigned char),
	       (unsigned long long) UCHAR_MAX);
	if (MAXUINT(unsigned char) != UCHAR_MAX) {
		puts("WARNING:  UINTMAX(unsigned char) != UCHAR_MAX !!!");
	}
	printf("MAX_INT(char)        = 0x%18llx, CHAR_MAX =   0x%18llx\n",
	       (unsigned long long) MAX_INT(char),
	       (unsigned long long) CHAR_MAX);
	if (MAX_INT(char) != CHAR_MAX) {
		puts("WARNING:  MAX_INT(char) != CHAR_MAX !!!");
	}
	printf("MAX_INT(signed char) = 0x%18llx, SCHAR_MAX =  0x%18llx\n",
	       (unsigned long long) MAX_INT(signed char),
	       (unsigned long long) SCHAR_MAX);
	if (MAX_INT(signed char) != SCHAR_MAX) {
		puts("WARNING:  MAX_INT(signed char) != SCHAR_MAX !!!");
	}
#if defined(__SCHAR_MAX__) && (__SCHAR_MAX__ != SCHAR_MAX)
	puts("WARNING:  __SCHAR_MAX__ != SCHAR_MAX !!!");
#endif
	printf("MAX_INT(u_char)      = 0x%18llx, UCHAR_MAX =  0x%18llx\n",
	       (unsigned long long) MAX_INT(u_char),
	       (unsigned long long) UCHAR_MAX);
	if (MAX_INT(unsigned char) != UCHAR_MAX) {
		puts("WARNING:  MAX_INT(unsigned char) != UCHAR_MAX !!!");
	}
#ifdef WEOF
	printf("MAX_INT(wchar_t)     = 0x%18llx, WCHAR_MAX =  0x%18llx\n",
	       (unsigned long long) MAX_INT(wchar_t),
	       (unsigned long long) WCHAR_MAX);
	if (MAX_INT(wchar_t) != WCHAR_MAX) {
		puts("WARNING:  MAX_INT(wchar_t) != WCHAR_MAX !!!");
	}
# if defined(__WCHAR_MAX__) && (__WCHAR_MAX__ != WCHAR_MAX)
	puts("WARNING:  __WCHAR_MAX__ != WCHAR_MAX !!!");
# endif
#endif
	printf("MAX_INT(short)       = 0x%18llx, SHRT_MAX  =  0x%18llx\n",
	       (unsigned long long) MAX_INT(short),
	       (unsigned long long) SHRT_MAX);
	if (MAX_INT(short) != SHRT_MAX) {
		puts("WARNING:  MAX_INT(short) != SHRT_MAX !!!");
	}
#if defined(__SHRT_MAX__) && (__SHRT_MAX__ != SHRT_MAX)
	puts("WARNING:  __SHRT_MAX__ != SHRT_MAX !!!");
#endif
	printf("MAX_INT(u_short)     = 0x%18llx, USHRT_MAX =  0x%18llx\n",
	       (unsigned long long) MAX_INT(unsigned short),
	       (unsigned long long) USHRT_MAX);
	if (MAX_INT(unsigned short) != USHRT_MAX) {
		puts("WARNING:  MAX_INT(unsigned short) != USHRT_MAX !!!");
	}
	printf("MAX_INT(int)         = 0x%18llx, INT_MAX   =  0x%18llx\n",
	       (unsigned long long) MAX_INT(int),
	       (unsigned long long) INT_MAX);
	if (MAX_INT(int) != INT_MAX) {
		puts("WARNING:  MAX_INT(INT) != INT_MAX !!!");
	}
#if defined(__INT_MAX__) && (__INT_MAX__ != INT_MAX)
	puts("WARNING:  __INT_MAX__ != INT_MAX !!!");
#endif
	printf("MAX_INT(u_int)       = 0x%18llx, UINT_MAX  =  0x%18llx\n",
	       (unsigned long long) MAX_INT(unsigned int),
	       (unsigned long long) UINT_MAX);
	if (MAX_INT(unsigned int) != UINT_MAX) {
		puts("WARNING:  MAX_INT(unsigned int) != UINT_MAX !!!");
	}
	printf("MAX_INT(long)        = 0x%18llx, LONG_MAX  =  0x%18llx\n",
	       (unsigned long long) MAX_INT(long),
	       (unsigned long long) LONG_MAX);
	if (MAX_INT(long) != LONG_MAX) {
		puts("WARNING:  MAX_INT(long) != LONG_MAX !!!");
	}
#if defined(__LONG_MAX__) && (__LONG_MAX__ != LONG_MAX)
	puts("WARNING:  __LONG_MAX__ != LONG_MAX !!!");
#endif
	printf("MAX_INT(u_long)      = 0x%18llx, ULONG_MAX =  0x%18llx\n",
	       (unsigned long long) MAX_INT(unsigned long),
	       (unsigned long long) ULONG_MAX);
	if (MAX_INT(unsigned long) != ULONG_MAX) {
		puts("WARNING:  MAX_INT(unsigned long) != ULONG_MAX !!!");
	}
	printf("MAX_INT(long long)   = 0x%18llx, LLONG_MAX =  0x%18llx\n",
	       (unsigned long long) MAX_INT(long long),
	       (unsigned long long) LLONG_MAX);
	if (MAX_INT(long long) != LLONG_MAX) {
		puts("WARNING:  MAX_INT(long long) != LLONG_MAX !!!");
	}
#if defined(__LONG_LONG_MAX__) && (__LONG_LONG_MAX__ != LLONG_MAX)
	puts("WARNING:  __LONG_LONG_MAX__ != LLONG_MAX !!!");
#endif
	printf("MAX_INT(u_longlong_t)= 0x%18llx, ULLONG_MAX=  0x%18llx\n",
	       (unsigned long long) MAX_INT(unsigned long long ),
	       (unsigned long long) ULLONG_MAX);
	if (MAX_INT(unsigned long long) != ULLONG_MAX) {
		puts("WARNING:  MAX_INT(unsigned long long) != ULLONG_MAX !!!");
	}
#if defined(__SIZEOF_INT128__)
	/* xxx hmmm.... */
#endif

	/* testing */
	printf("MAX_INT(quad_t)      = 0x%18llx\n",
	       (unsigned long long) MAX_INT(quad_t));
	printf("MAX_INT(u_quad_t)    = 0x%18llx\n",
	       (unsigned long long) MAX_INT(u_quad_t));

	printf("MAX_INT(time_t)      = 0x%18llx\n",
	       (unsigned long long) MAX_INT(time_t));
	printf("MAX_INT(off_t)       = 0x%18llx\n",
	       (unsigned long long) MAX_INT(off_t));
	printf("MAX_INT(fpos_t)      = 0x%18llx\n",
	       (unsigned long long) MAX_INT(fpos_t));
	printf("MAX_INT(size_t)      = 0x%18llx\n",
	       (unsigned long long) MAX_INT(size_t));
	printf("MAX_INT(ssize_t)     = 0x%18llx\n",
	       (unsigned long long) MAX_INT(ssize_t));

	/* sign extension for hex */
	printf("MINSINT(char)        = 0x%18llx, aka with sign extension =    %lld\n",
	       (long long) MINSINT(char), (long long) MINSINT(char));

	/* There but for the want of a signed hex formatter I go! */
	printf("MINSINT(char)        = %20lld, CHAR_MIN =   %20lld\n",
	       (long long) MINSINT(char), (long long) CHAR_MIN);
	if (MINSINT(char) != CHAR_MIN) {
		puts("WARNING:  MINSINT(char) != CHAR_MIN !!!");
	}

	printf("MAXSINT(char)        = %20lld, CHAR_MAX =   %20lld\n",
	       (long long) MAXSINT(char), (long long) CHAR_MAX);
	if (MAXSINT(char) != CHAR_MAX) {
		puts("WARNING:  MAXSINT(char) != CHAR_MAX !!!");
	}

	printf("MINSINTx(signed char)= %20lld, SCHAR_MIN =  %20lld\n",
	       (long long) MINSINTx(signed char), (long long) SCHAR_MIN);
	if (MINSINTx(signed char) != SCHAR_MIN) {
		puts("WARNING:  MINSINTx(signed char) != SCHAR_MIN !!!");
	}

	printf("MAX_INT(signed char) = %20lld, SCHAR_MAX =  %20lld\n",
	       (long long) MAX_INT(signed char), (long long) SCHAR_MAX);
	if (MAX_INT(signed char) != SCHAR_MAX) {
		puts("WARNING:  MAX_INT(signed char) != SCHAR_MAX !!!");
	}

#ifdef WEOF
	printf("MINSINTx(wchar_t)    = %20lld, WCHAR_MIN =  %20lld\n",
	       (long long) MINSINTx(wchar_t), (long long) WCHAR_MIN);
	if (MINSINTx(wchar_t) != WCHAR_MIN) {
		puts("WARNING:  MINSINTx(wchar_t) != WCHAR_MIN !!!");
	}

	printf("MAX_INT(wchar_t)     = %20lld, WCHAR_MAX =  %20lld\n",
	       (long long) MAX_INT(wchar_t), (long long) WCHAR_MAX);
	if (MAX_INT(wchar_t) != WCHAR_MAX) {
		puts("WARNING:  MAX_INT(wchar_t) != WCHAR_MAX !!!");
	}
#endif

	printf("MINSINT(short)       = %20lld, SHRT_MIN  =  %20lld\n",
	       (long long) MINSINT(short), (long long) SHRT_MIN);
	if (MINSINT(short) != SHRT_MIN) {
		puts("WARNING:  MINSINT(short) != SHRT_MIN !!!");
	}

	printf("MAXSINT(short)       = %20lld, SHRT_MAX  =  %20lld\n",
	       (long long) MAXSINT(short), (long long) SHRT_MAX);
	if (MAXSINT(short) != SHRT_MAX) {
		puts("WARNING:  MAXSINT(short) != SHRT_MAX !!!");
	}

	printf("MINSINT(int)         = %20lld, INT_MIN   =  %20lld\n",
	       (long long) MINSINT(int), (long long) INT_MIN);
	if (MINSINT(int) != INT_MIN) {
		puts("WARNING:  MINSINT(int) != INT_MIN !!!");
	}
	printf("MAXSINT(int)         = %20lld, INT_MAX   =  %20lld\n",
	       (long long) MAXSINT(int), (long long) INT_MAX);
	printf("MINSINT(long)        = %20lld, LONG_MIN  =  %20lld\n",
	       (long long) MINSINT(long), (long long) LONG_MIN);
	if (MINSINT(long) != LONG_MIN) {
		puts("WARNING:  MINSINT(long) != LONG_MIN !!!");
	}
	printf("MAXSINT(long)        = %20lld, LONG_MAX  =  %20lld\n",
	       (long long) MAXSINT(long), (long long) LONG_MAX);
	printf("MINSINT(long long)   = %20lld, LLONG_MIN =  %20lld\n",
	       (long long) MINSINT(long long), (long long) LLONG_MIN);
	if (MINSINT(long long) != LLONG_MIN) {
		puts("WARNING:  MINSINT(long long) != LLONG_MIN !!!");
	}
	printf("MAXSINT(long long)   = %20lld, LLONG_MAX =  %20lld\n",
	       (long long) MAXSINT(long long), (long long) LLONG_MAX);

	exit(0);
}

/*
 * CPPFLAGS+="-fsigned-char"
 * CPPFLAGS+="-funsigned-char"
 *
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
