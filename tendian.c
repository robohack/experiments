/*
 *	testing for processor endian-ness
 *
 * See tbyteorder.c for why you should never really need to do this.
 *
 * Big endian machine:  Stores data big-end first.  When looking at multiple
 * bytes, the first byte (lowest address) is the biggest.
 *
 * Little endian machine:  Stores data little-end first.  When looking at
 * multiple bytes, the first byte is smallest.
 *
 *	0x12345678	<- integer value
 *
 * Big-endian addressing by byte:
 *	a:	0x12
 *	a+1:	0x34
 *	a+2:	0x56
 *	a+3:	0x78
 *
 * Little-endian addressing by byte:
 *	a:	0x78
 *	a+1:	0x56
 *	a+2:	0x34
 *	a+3:	0x12
 *
 * And then there was the PDP-11, sort of:
 *	a:	0x34
 *	a+1:	0x12
 *
 * The PDP-11 does not have instructions to store 32-bit values to memory.  It
 * stores 16-bit values to memory in the usual little endian manner.  It can do
 * 32-bit arithmetic though, by storing the values in pairs of 16-bit CPU
 * registers.  The most signficant word goes into the lower numbered register,
 * but within each register values are stored little-endian.  Code that blindly
 * copies the registers in order out to memory could generate values with the
 * PDP-11 ordering above.
 *
 * NOTE:  This can also be done in a fashion from the shell with the following
 * trick:
 *
 *	printf '\x01\x02\x03\x04' | od -t x | sed 's/0/ /g;q'
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
#endif

#ifdef HAVE_STDBOOL_H
# include <stdbool.h>
#else /* !HAVE_STDBOOL_H  */
typedef enum bool_e { false = 0U, true = !false } bool;
#endif /* !HAVE_STDBOOL_H  */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* macros to extract little-endian integers from an byte buffer */
#define PULL2(p)	((p)[0] + ((uint16_t) ((p)[1]) << 8))
#define PULL4(p)	(PULL2(p) + ((uint32_t) PULL2(p + 2) << 16))
#define PULL8(p)	(PULL4(p) + ((uint64_t) PULL4(p + 4) << 32))

/* macros to store integers into a byte buffer in little-endian order */
#define PUT2(p, a)	do {(p)[0] = a; (p)[1] = (a) >> 8;} while(0)
#define PUT4(p, a)	do {PUT2(p, a); PUT2(p + 2, a >> 16);} while(0)
#define PUT8(p, a)	do {PUT4(p, a); PUT4(p + 4, a >> 32);} while(0)

/*
 * a trick using compile-time initialization to determine endianness and
 * including tests for odd-balls with middle-endianness too (though can only be
 * used at run-time of course, but that means it works for bi-endian CPUs too!)
 *
 * from http://stackoverflow.com/a/2103095/816536
 */
enum {
	O32_BIG_ENDIAN =		0x00010203ul,	/* ABCD */
	O32_BIG_ENDIAN_WORD_SW =	0x02030001ul,	/* Honeywell 316, CDAB */
	O32_LITTLE_ENDIAN =		0x03020100ul,	/* DCBA */
	O32_LITTLE_ENDIAN_WORD_SW =	0x01000302ul,	/* PDP-11, BADC */
};
/*
 * N.B.:  This implementation uses exact-width integer types.  These types are
 * optional if the underlying hardware does not support them.  In that case this
 * code will likely fail to compile.  If you want this to work for really
 * odd-ball architectures, then use the corresponding minimum-width integer
 * types, e.g. uint_least8_t and uint_least32_t, and return ENDIAN_UNKNOWN if
 * (sizeof(uint_least8_t) != 1 && sizeof(uint_least32_t) != 4)
 */
static const union {
	uint8_t bytes[4];
	uint32_t value;
} o32_host_order =     { { 0, 1, 2, 3 } };

static bool
o32_is_little_endian(void)
{
	return (o32_host_order.value == O32_LITTLE_ENDIAN);
}
static bool
o32_is_big_endian(void)
{
	return (o32_host_order.value == O32_BIG_ENDIAN);
}

/* newer GCC & Clang will already have these defined... */
#if !(defined(__LITTLE_ENDIAN__) || defined(__BIG_ENDIAN__))

# if !(defined(__BYTE_ORDER__) &&					\
       defined(__ORDER_LITTLE_ENDIAN__) && defined(__ORDER_BIG_ENDIAN__))

#  if (defined(linux) || defined(__linux__) ||                          \
       defined (__gnu_linux__) || defined(__linux))
#   include <endian.h>
#  endif

#  if (defined(BSD) ||                                                  \
       defined(__NetBSD__) ||                                           \
       defined(__FreeBSD__) ||                                          \
       defined(__OpenBSD__) ||                                          \
       defined(__Darwin__) ||                                           \
       defined(__DragonFly__))
#   include <sys/types.h>
#  endif

#  ifdef __SCO__
#   include <sys/types.h>
#   include <sys/byteorder.h>
#  endif

#  ifdef __aix__
#   include <sys/machine.h>
#  endif

#  ifdef __hpux
#   include <machine/param.h>
#  endif

#  ifdef OS2
#   include <machine/endian.h>
#  endif

#  ifdef SVR4
#   if defined(NCR) || defined(Mips) || defined(__sgi)
#    include <sys/endian.h>
#   else
#    if !defined(sun)
#     include <sys/byteorder.h>
#    endif
#   endif
#  endif

/* the above #includes should define one of the following groups... */

#  if !defined(__ORDER_LITTLE_ENDIAN__) && defined(_LITTLE_ENDIAN)
#   define __ORDER_LITTLE_ENDIAN__	_LITTLE_ENDIAN
#  endif
#  if !defined(__ORDER_BIG_ENDIAN__) && defined(_BIG_ENDIAN)
#   define __ORDER_BIG_ENDIAN__		_BIG_ENDIAN
#  endif
#  if !defined(__BYTE_ORDER__) && defined(_BYTE_ORDER)
#   define __BYTE_ORDER__		_BYTE_ORDER
#  endif

#  if !defined(__ORDER_LITTLE_ENDIAN__) && defined(__LITTLE_ENDIAN)
#   define __ORDER_LITTLE_ENDIAN__	__LITTLE_ENDIAN
#  endif
#  if !defined(__ORDER_BIG_ENDIAN__) && defined(__BIG_ENDIAN)
#   define __ORDER_BIG_ENDIAN__		__BIG_ENDIAN
#  endif
#  if !defined(__BYTE_ORDER__) && defined(__BYTE_ORDER)
#   define __BYTE_ORDER__		__BYTE_ORDER
#  endif

/*
 * If we still dont know the target byte order, then we try one last effort.
 *
 * This is based on compiler CPU macros, so for bit-field determination it's no
 * better than those above, but for code wanting to know the word endianness...
 */
#  ifndef __BYTE_ORDER__

/*
 * note: these are the values GCC & Clang use....
 */
#   ifndef __ORDER_LITTLE_ENDIAN__
#    define __ORDER_LITTLE_ENDIAN__	1234
#   endif
#   ifndef __ORDER_BIG_ENDIAN__
#    define __ORDER_BIG_ENDIAN__   	4321
#   endif

#   if defined(i386) || defined(__i386__) || defined(__x86__) ||	\
	defined(vax) || defined(__vax__) ||                             \
	defined(_MIPSEL) ||defined(__MIPSEL) || defined(__MIPSEL__) ||  \
	defined(__ARMEL__) || defined(__THUMBEL__) ||			\
	defined(__AARCH64EL__)
#    define __BYTE_ORDER__	__ORDER_LITTLE_ENDIAN__
#   else
/* assume there is no need to support middle-endian */
#    define __BYTE_ORDER__	__ORDER_BIG_ENDIAN__
#   endif

#   if !defined(__BYTE_ORDER__) && defined(_LITTLE_ENDIAN)
#    define __BYTE_ORDER__	__ORDER_LITTLE_ENDIAN__
#   endif
#   if !defined(__BYTE_ORDER__) && defined(_BIG_ENDIAN)
#    define __BYTE_ORDER__	__ORDER_BIG_ENDIAN__
#   endif

#  endif /* __BYTE_ORDER__ */

# endif /* !(__BYTE_ORDER__ && __ORDER_LITTLE_ENDIAN__ && __ORDER_BIG_ENDIAN__)) */

# if !defined(__BIG_ENDIAN__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#  define __BIG_ENDIAN__
# elif !defined(__LITTLE_ENDIAN__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#  define __LITTLE_ENDIAN__
# endif

#endif /* !(__LITTLE_ENDIAN__) || __BIG_ENDIAN__) */

static volatile uint32_t l = 0x01020304UL;
volatile uint8_t *pl = (volatile uint8_t *) &l;

union {
	uint32_t x;
	char c[sizeof(uint32_t)];
} z = {
	/* initialize integer value using characters */
	.x = ('1' << 24) | ('2' << 16) | ('3' << 8) | '4'
};

#ifdef __LP64__
static volatile uint64_t ll = 0x0102030405060708ULL;
volatile uint8_t *pll = (volatile uint8_t *) &ll;

union {
	uint64_t x;
	char c[sizeof(uint64_t)];
} y = {
	/* initialize integer value using characters */
	.x = ((uint64_t) '1' << 56) | ((uint64_t) '2' << 48) | ((uint64_t) '3' << 40) | ((uint64_t) '4' << 32) | ((uint64_t) '5' << 24) | ((uint64_t) '6' << 16) | ((uint64_t) '7' << 8) | (uint64_t) '8'
};
#endif

/*
 * These runtime tests usually do not generate any code at all when even minimal
 * optimisation enabled, such as "gcc -O1", and since they are declared as
 * static inline, you can even safely put them directly in a header file.
 */
static inline unsigned int
is_big_endian(void)
{
	const uint_least16_t endianness = 256;

	return (1 == *(const uint8_t *) &endianness);
}

static inline unsigned int
is_little_endian(void)
{
	const uint_least16_t endianness = 1;

	return (1 == *(const unsigned char *) &endianness);
}


int main(void);

int
main()
{
	int i;

	printf("This is running on %s endian.\n",
	       is_big_endian() ? "BIG" : "LITTLE");

	printf("This really is running on %s endian.\n",
	       is_little_endian() ? "LITTLE" : "BIG");

	printf("again, this is running on %s endian.\n",
	       o32_is_big_endian() ? "BIG" : "LITTLE");

	printf("finally, this really is running on %s endian.\n",
	       o32_is_little_endian() ? "LITTLE" : "BIG");

	/* print the byte order shown in the char array */
	printf("%*.*s\n", (int) sizeof(z.c), (int) sizeof(z.c), z.c);
#ifdef __LP64__
	printf("%*.*s\n", (int) sizeof(y.c), (int) sizeof(y.c), y.c);
#endif

	switch (*pl) {
	case 0x01:
		printf("BIG-Endian\n");
#ifndef __BIG_ENDIAN__
		printf("Hmmm...  BIG-Endian _not_ detected at compile time!\n");
#endif
#ifdef __LITTLE_ENDIAN__
		printf("OH OH!  LITTLE-Endian detected at compile time!!!\n");
#endif
		break;
	case 0x04:
		printf("LITTLE-Endian\n");
#ifndef __LITTLE_ENDIAN__
		printf("Hmmm... LITTLE-Endian _not_ detected at compile time!\n");
#endif
#ifdef __BIG_ENDIAN__
		printf("OH OH!  BIG-Endian detected at compile time!!!\n");
#endif
		break;
	case 0x02:
		printf("PDP-11\n");
#ifdef __BIG_ENDIAN__
		printf("OH OH!  BIG-Endian detected at compile time!!!\n");
#endif
#ifdef __LITTLE_ENDIAN__
		printf("OH OH!  LITTLE-Endian detected at compile time!!!\n");
#endif
		break;
	default:
		printf("WTF!?!?!?\n");
		break;
	}

	/* and just for comparison, print the bytes in the integer 'l' */
	printf("l = 0x%8.8x\n", l);
	for (i = 0; i < (int) sizeof(l); i++) {
		printf("l[%i] = %x%s", i, *pl++, i == sizeof(l)-1 ? "\n" : ", ");
	}
#ifdef __LP64__
	printf("ll = 0x%16.16llx\n", (unsigned long long) ll);
	for (i = 0; i < (int) sizeof(ll); i++) {
		printf("ll[%i] = %x%s", i, *pll++, i == sizeof(ll)-1 ? "\n" : ", ");
	}
#endif

	l = 0xF01010F0;
	printf("l = 0x%8.8x\n", l);
	pl = (volatile uint8_t *) &l;
	for (i = 0; i < (int) sizeof(l); i++) {
		printf("l[%i] = \\%03o%s", i, *pl++, i == sizeof(l)-1 ? "\n" : ", ");
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
