/*
 * Vast amounts of information and advice about floating point and such related
 * issues is available in the "Numerical Computation Guide" from Oracle (nee Sun
 * Microsystems), especially the Appendix titled "What Every Computer Scientist
 * Should Know About Floating-Point Arithmetic" (aka WECSSKAFPA)
 *
 *	http://docs.oracle.com/cd/E19957-01/806-3568/ncg_goldberg.html
 *
 * See also Bill Kahan's web page:  http://www.cs.berkeley.edu/~wkahan/
 *
 * See also:  http://speleotrove.com/decimal/decifaq.html
 *
 * Ryan Juckett has some good descriptions and analysis of the algorithms
 * printing floating point numbers here and on pages that follow it:
 *
 *	http://www.ryanjuckett.com/programming/printing-floating-point-numbers/
 *
 * See also Rick Regan's pages at:  http://www.exploringbinary.com/
 * (Rick is very analytical and goes far beyond the practical issues.)
 *
 * More floating point headaches, this series of essays by Bruce Dawson:
 *
 *	https://randomascii.wordpress.com/category/floating-point/
 *
 * One of his interesting, but sillier, tricks is how to keep microsecond
 * precision when storing time (e.g. elapsed time) in a double.  Start with the
 * start time (your effective "zero time", or "epoch") equal to 2^32 seconds.
 * This should keep better-than-microsecond precision for time periods from zero
 * seconds and over 100 years.  This trick works because normal floating point
 * is far more precise for values near zero.  He says "Any time you add or
 * subtract float[ing point value]s of widely varying magnitudes you need to
 * watch for loss of precision."  So, starting the "epoch" for such timestamps
 * at 2^32 seconds eliminates these difference of magnitudes when calculating
 * elapsed time, no matter if it is a few miroseconds later, or 50 years later.
 *
 * The floating-point representation of elapsed time has the advantage that we
 * get sub-second precision 'free', which for more contemporary dates is about a
 * millionth of a second.
 *
 * Of course a signed 64-bit integer, with zero set as its epoch to any date in
 * the modern era, gives us +/- 292277 years at microsecond resolution.  Even
 * 2^31 decaseconds (1/10s) is over 680 years.
 *
 * So, DO NOT actually use this stilly trick of storing and calculating
 * timestamps and elapsed time in floating point unless you have a much faster
 * floating point unit than your (equivalent) 64-bit ALU is, or MAYBE if you
 * need to represent geological timespans but still have fine precision near
 * some specific epoch.
 */

/* xxx build with -std=c99 or newer */
#if (defined(__STDC_VERSION__) && ((__STDC_VERSION__ - 0) >= 199901L))
# define _ISOC99_SOURCE	1
# define _ISOC9X_SOURCE	1
#endif

#if (defined(BSD4_4) ||							\
     defined(__NetBSD__) ||                                             \
     defined(__FreeBSD__) ||                                            \
     defined(__OpenBSD__) ||                                            \
     defined(__Darwin__) ||                                             \
     defined(__DragonFly__) ||						\
     defined(__APPLE__) ||						\
     defined(__linux__))
# include <sys/cdefs.h>
# include <sys/param.h>
/* also <sys/types.h>??? */
#endif
#if ((defined(BSD4_4) &&						\
      !defined(__APPLE__) &&						\
      !defined(__FreeBSD__)) ||						\
     defined(__NetBSD__) ||						\
     defined(__OpenBSD__) ||                                            \
     defined(__DragonFly__) ||	/* ??? */				\
     defined(__linux__) ||	/* ??? */				\
     defined(__ANDROID__))
# include <machine/ieee.h>
#endif
#include <sys/utsname.h>

#if (!defined(HAVE_STDBOOL_H) &&                                        \
     ((defined(__STDC_VERSION__) &&                                     \
       ((__STDC_VERSION__ - 0) >= 199901L)) ||                          \
      (defined(__NetBSD_Version__) &&                                   \
       ((__NetBSD_Version__ - 0) >= 106000000)) ||                      \
      (defined(__APPLE_CC__) && defined(__STDC__)) ||                   \
      defined(__linux__)))
/* n.b. there were no doubt versions of GNU/linux without stdbool.h */
# define HAVE_STDBOOL_H
#endif

#include <float.h>
#include <inttypes.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* XXX I wish apple would stop pretending so hard... */
#if ((defined(BSD4_4) &&						\
      !defined(__APPLE__)) ||						\
     defined(__NetBSD__) ||                                             \
     (defined(__FreeBSD__) &&						\
      !defined(__APPLE__)) ||						\
     defined(__OpenBSD__) ||                                            \
     defined(__Darwin__) ||                                             \
     defined(__DragonFly__))
# include <ieeefp.h>
#endif

#ifdef HAVE_STDBOOL_H
# include <stdbool.h>
#else /* !HAVE_STDBOOL_H  */
typedef enum bool_e { false = 0U, true = !false } bool;
#endif /* !HAVE_STDBOOL_H  */

/*
 * N.B.:  disable this to see un-rounded values at max output precision from
 * print_decimal(), i.e. the precision necessary for binary -> decimal -> binary
 * conversion.  (when enabled it preserves for decimal -> binary -> decimal
 * conversions)
 */
#define PRINT_ROUND_TRIP_SAFE		/* defined */

#ifndef DECIMAL_DIG			/* added in C11 */
# ifdef __DECIMAL_DIG__			/* also from GCC */
#  define DECIMAL_DIG __DECIMAL_DIG__	/* osx 10.6.8 borkage */
# endif
#endif

#define calc_FLT_DECIMAL_DIG	((int) lrint(ceil(FLT_MANT_DIG * log10((double) FLT_RADIX))) + 1)
#define calc_DBL_DECIMAL_DIG	((int) lrint(ceil(DBL_MANT_DIG * log10((double) FLT_RADIX))) + 1)
#define calc_LDBL_DECIMAL_DIG	((int) lrint(ceil(LDBL_MANT_DIG * log10((double) FLT_RADIX))) + 1)

#ifdef FLT_DECIMAL_DIG			/* added in C11 */
# define my_FLT_DECIMAL_DIG	FLT_DECIMAL_DIG
#else
# define my_FLT_DECIMAL_DIG	calc_FLT_DECIMAL_DIG
#endif

#ifdef DBL_DECIMAL_DIG			/* added in C11 */
# define my_DBL_DECIMAL_DIG	DBL_DECIMAL_DIG
#else
# define my_DBL_DECIMAL_DIG	calc_DBL_DECIMAL_DIG
#endif

#ifdef LDBL_DECIMAL_DIG			/* added in C11 */
# define my_LDBL_DECIMAL_DIG	LDBL_DECIMAL_DIG
#else
# define my_LDBL_DECIMAL_DIG	calc_LDBL_DECIMAL_DIG
#endif

/*
 * *_STRING_SIZE:  not yet used herin, but shown as an example
 */
/*                       - d .  dddddddd              e - dddd \0 */
#define FLT_STRING_SIZE (1+1+1+(my_FLT_DECIMAL_DIG-1)+1+1+ 4   +1)
/*                       - d .  dddddddddddddddd      e - dddd \0 */
#define DBL_STRING_SIZE (1+1+1+(my_DBL_DECIMAL_DIG-1)+1+1+ 4   +1)
/*                        - d .  dddddddddddddddddddd   e - dddd \0 */
#define LDBL_STRING_SIZE (1+1+1+(my_LDBL_DECIMAL_DIG-1)+1+1+ 4   +1)

#if !defined(HAVE_FENV_H) && !defined(HAVE_IEEEFP_H)
# if defined(__NetBSD__)
#  if defined(__NetBSD_Prereq__)
#   if __NetBSD_Prereq__(6, 0, 0)
#    define HAVE_FENV_H			/* defined */
#    define HAVE_FEGETROUND		/* defined */
#   else
#    define HAVE_IEEEFP_H		/* defined */
#    define HAVE_FPGETROUND		/* defined */
#   endif
#  endif
# endif	 /* __NetBSD__ */
/* xxx for other BSDs which are not yet at C99? */
# if defined(__linux__) && defined(__GLIBC_PREREQ)
#  if __GLIBC_PREREQ(2,1)
#   define HAVE_FENV_H			/* defined */
#   define HAVE_FEGETROUND		/* defined */
#  else
    /* xxx before glibc-2.1??? */
#  endif
# endif	 /* __linux__ */
#endif

#if (defined(HAVE_FENV_H) ||                                            \
     (! defined(HAVE_IEEEFP_H) &&                                       \
      (defined(__STDC_VERSION__) &&                                     \
       ((__STDC_VERSION__ - 0) >= 199901L))))
# include <fenv.h>
#else
# include <ieeefp.h>
#endif

#if defined(__NetBSD__) && defined(__NetBSD_Prereq__)
# if __NetBSD_Prereq__(7, 0, 0)
#  define HAVE_FLOORL
#  define HAVE_LOG10L
#  define HAVE_ROUNDL
#  if __NetBSD_Prereq__(8, 99, 0) && !__NetBSD_Prereq__(9, 0, 0)
/* XXX this was present temporarily in 8.99.x, but then "accidentally" removed again */
/* XXX the main problem was lack of the "#include <math.h>" in src/lib/libm/src/s_lrintl.c */
//#   define HAVE_LRINTL
#  endif
#  if __NetBSD_Prereq__(10, 99, 3)	/* actual: 2023-03-13 */
#   define HAVE_LRINTL
#  endif
# else
/* n.b.  it works on 5.2, apparently courtesy GCC builtin functions */
long double powl(long double x, long double y);
long double fabsl(long double n);	/* not as well tested */
# endif
#endif

#if defined(__FreeBSD__)
# if __FreeBSD_version >= 600000
# define HAVE_FLOORL			/* ??? */
# define HAVE_LOG10L			/* ??? */
#  define HAVE_ROUNDL
# endif
# if __FreeBSD_version >= 800000
#  define HAVE_LRINTL
# endif
#endif

#if !defined(HAVE_ROUNDL) && defined(_POSIX_C_SOURCE) && (_POSIX_C_SOURCE - 0) >= 200112L
# define HAVE_FLOORL
# define HAVE_LOG10L
# define HAVE_ROUNDL
#endif

#if !defined(HAVE_ROUNDL) && defined(__APPLE__) && defined(__LDBL_DIG__) /* xxx why did I add __LDBL_DIG__ */
# define HAVE_FLOORL
# define HAVE_LOG10L
# define HAVE_ROUNDL
#endif

/* newer GCC & Clang will already have these defined... */
#if !(defined(__LITTLE_ENDIAN__) || defined(__BIG_ENDIAN__))

# if !(defined(__BYTE_ORDER__) &&					\
	defined(__ORDER_LITTLE_ENDIAN__) && defined(__ORDER_BIG_ENDIAN__))

#  if defined(linux) || defined(__linux__) ||				\
	defined (__gnu_linux__) || defined(__linux)
#   include <endian.h>
#  endif

#  if defined(BSD) ||							\
	defined(__NetBSD__) ||						\
	defined(__FreeBSD__) ||						\
	defined(__OpenBSD__) ||						\
	defined(__Darwin__) ||						\
	defined(__DragonFly__)
#   include <sys/param.h>
#   include <machine/endian.h>
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

/* the above #includes should define one of the following... */

#  if !defined(__ORDER_LITTLE_ENDIAN__) && defined(_LITTLE_ENDIAN)
#   define __ORDER_LITTLE_ENDIAN__	_LITTLE_ENDIAN
#  endif
#  if !defined(__ORDER_BIG_ENDIAN__) && defined(_BIG_ENDIAN)
#   define __ORDER_BIG_ENDIAN__	_BIG_ENDIAN
#  endif
#  if !defined(__BYTE_ORDER__) && defined(_BYTE_ORDER)
#   define __BYTE_ORDER__		_BYTE_ORDER
#  endif

#  if !defined(__ORDER_LITTLE_ENDIAN__) && defined(__LITTLE_ENDIAN)
#   define __ORDER_LITTLE_ENDIAN__	__LITTLE_ENDIAN
#  endif
#  if !defined(__ORDER_BIG_ENDIAN__) && defined(__BIG_ENDIAN)
#   define __ORDER_BIG_ENDIAN__	__BIG_ENDIAN
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

#   if defined(__QNX__) && !defined(__QNXNTO__)
#    define __BYTE_ORDER__	__ORDER_LITTLE_ENDIAN__
#   endif  /* __QNX__ */

#   if defined(__QNXNTO__)
#    if defined(i386) || defined(__i386__) || defined(__x86__)
#     define __BYTE_ORDER__	__ORDER_LITTLE_ENDIAN__
#    else
#     define __BYTE_ORDER__	__ORDER_BIG_ENDIAN__
#    endif
#   endif  /* __QNXNTO__ */

#   ifdef Lynx
#    if defined(i386) || defined(__i386__) || defined(__x86__)
#     define __BYTE_ORDER__	__ORDER_LITTLE_ENDIAN__
#    else
#     define __BYTE_ORDER__	__ORDER_BIG_ENDIAN__
#    endif
#   endif  /* Lynx */

#   if !defined(__BYTE_ORDER__) && defined(_LITTLE_ENDIAN)
#    define __BYTE_ORDER__	__ORDER_LITTLE_ENDIAN__
#   endif
#   if !defined(__BYTE_ORDER__) && defined(_BIG_ENDIAN)
#    define __BYTE_ORDER__	__ORDER_BIG_ENDIAN__
#   endif

#  endif /* __BYTE_ORDER__ */

# endif /* !(__BYTE_ORDER__ && __ORDER_LITTLE_ENDIAN__ && __ORDER_BIG_ENDIAN__)) */

# if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#  define __BIG_ENDIAN__
# elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#  define __LITTLE_ENDIAN__
# endif

#endif /* !(__LITTLE_ENDIAN__) || __BIG_ENDIAN__) */

#if !(defined(__LITTLE_ENDIAN__) || defined(__BIG_ENDIAN__))
# error "one of __LITTLE_ENDIAN__ or __BIG_ENDIAN__ must be defined"
#endif

/*
 * Determine the compiler's bitfield order....
 *
 * Note that it is assumed the host has a two's-complement integer
 * representation, and that the compiler can allocate bitfields into 32-bit
 * storage unit
 *
 * The code below does not make any assumptions about the byte order of the
 * host.  With most compiler implementations the allocation of the bitfields in
 * each storage unit is done in the same order as bytes are allocated within
 * multibyte objects.  Since we disect bitfields from externally created data,
 * we must take care to do it in a manner compatible with the compiler.
 *
 *
 * Bit-fields and their order in structs.
 *
 * From C99:
 *
 *	An implementation may allocate any addressable storage unit large
 *	enough to hold a bit-field.
 *
 *	If enough space remains, a bit-field that immediately follows another
 *	bit-field in a structure shall be packed into adjacent bits of the same
 *	unit.
 *
 *	If insufficient space remains, whether a bit-field that does not fit is
 *	put into the next unit or overlaps adjacent units is
 *	implementation-defined.
 *
 * Some architectures like SPARC and Motorola 68000 use "LSB 0" bit numbering,
 * while S/390, PowerPC and PA-RISC use "MSB 0".  Dunno about their bit-fields
 * though.
 *
 * GCC's manual discusses the layout of bit-fields in structures, mentioning
 * the sections of the relevant C standards which say that this aspect of C is
 * "implementation defined", and then says how GCC implements the feature:
 *
 *   * `The order of allocation of bit-fields within a unit (C90 6.5.2.1,
 *      C99 6.7.2.1).'
 *
 *   Determined by ABI.
 *
 *   Some choices depend on the externally determined ABI for the platform
 *   which GCC follows
 *
 * Some compilers, e.g. reportedly Metrowerks CodeWarrior, support a "#pragma
 * reverse_bitfields" directive that will reverse the order of the bits in a
 * bit-field from the order that would otherwise have applied.
 *
 * The COSMIC compiler for HC12/HCS12 microcontrollers defines __CSMC__ to a
 * value with 6 or more bits, of which bit #4 is set if the "reverse bitfield
 * option (was) specified (+rev)".  (Apparently it normally uses LtoH
 * ordering.)
 *
 * The UNIX System V ABI for PowerPC says that the CPU may be used in either
 * Little Endian mode or Big Endian mode, and with respect to bit-fields:
 *
 *	Bit-fields are allocated from right to left (least to most significant)
 *	on Little-Endian implementations and from left to right (most to least
 *	significant) on Big-Endian implementations.
 *
 * For i386, the SysV ABI says:
 *
 *	Bit-fields are allocated from right to left (least to most significant).
 *
 * For all Sun Sparc:
 *
 * 	The C compiler assigns bit fields from the high to the low end of an
 * 	int (most to least significant).
 *
 * However for all Sun systems we can get the official value directly from
 * <sys/isa_defs.h> as a define of either _BIT_FIELDS_LTOH or _BIT_FIELDS_HTOL.
 */
#if defined(__sun__)
# include <sys/isa_defs.h>
#endif

#if !(defined(_BIT_FIELDS_LTOH) || defined(_BIT_FIELDS_HTOL))
/*
 * First try by CPU based on compiler defines....
 */
# if defined(sun3) || defined(mc68000) ||				\
    defined(sun4) || defined(__sparc) || defined(sparc) ||		\
    defined(__hppa) || defined(_ARCH_PPC) || defined(_IBMR2)
#  define _BIT_FIELDS_HTOL
# endif

# if defined(__sgi) && defined(__mips)
#  define _BIT_FIELDS_HTOL
# endif

# if defined(__i386__) || defined(__i386) || defined(i386) ||		\
    defined(__alpha__) || defined(__alpha) || defined(alpha) ||		\
    defined(__arm__) || defined(__arm) || defined(arm)
#  define _BIT_FIELDS_LTOH
# endif

#endif /* !(_BIT_FIELDS_LTOH || _BIT_FIELDS_HTOL) */

/* endian hacks... */
#if !(defined(_BIT_FIELDS_LTOH) || defined(_BIT_FIELDS_HTOL))
/*
 * If that didn't work then try using platform-specific compile-time endian
 * discovery is a last-ditch effort.  This will likely only work on hosted
 * implementations.
 */
# if defined(__LITTLE_ENDIAN__)
#  define _BIT_FIELDS_LTOH
# elif defined(__BIG_ENDIAN__)
#  define _BIT_FIELS_HTOL
# endif

# if !(defined(_BIT_FIELDS_LTOH) || defined(_BIT_FIELDS_HTOL))
#  include "one of _BIT_FIELDS_LTOH or _BIT_FIELDS_HTOL must be defined"
# endif

#endif /* endian hacks */

/* xxx with the __ prefix these should probably be protected with #ifndef */
#define	___STRING(x)	__STRING(x)
#define	__STRING(x)	#x

#define round2int(x) (((x) < (double)INT_MIN-0.5 || (x) > (double)INT_MAX+0.5) ? abort(),0 : ((x)>=0?(int)((x)+0.5):(int)((x)-0.5)))
#define round2long(x) (((x) < (double)LONG_MIN-0.5 || (x) > (double)LONG_MAX+0.5) ? abort(),0 : ((x)>=0?(long)((x)+0.5):(long)((x)-0.5)))
#define round2max(x) (((x) < (double)INTMAX_MIN-0.5 || (x) > (double)INTMAX_MAX+0.5) ? abort(),0 : ((x)>=0?(intmax_t)((x)+0.5):(intmax_t)((x)-0.5)))

/* FYI */
#define CEILING_POS(X)	((X-(int)(X)) > 0 ? (int)(X+1) : (int)(X))
#define CEILING_NEG(X)	((X-(int)(X)) < 0 ? (int)(X-1) : (int)(X))
#define CEILING(X)	(((X) > 0) ? CEILING_POS(X) : CEILING_NEG(X))


int main(void);

static void report(char *);
static void representations(void);
static void precision(void);

static intmax_t ipow(int base,
		     unsigned int expon);

static uintmax_t uipow(unsigned int base,
		       unsigned int expon);

int
main()					/* xxx clang is getting mouthy */
{
	struct utsname utsname;

	uname(&utsname);

	report(utsname.machine);

	representations();

	precision();

	exit(0);
	/* NOTREACHED */
}

char *binary_fmt(uintmax_t, int);
char *
binary_fmt(uintmax_t x,
           int zf)			/* natural promoted type of bool */
{
	static char str[(sizeof(x) * CHAR_BIT) + 1];
	char *s = str + sizeof(str) - 1;

	*str = *s = '\0';
	do {
#if 0	/* oh wonderful compiler, which is best? */
		*--s = (char) ('0' + (x % 2));
#else
# if (defined(__STDC_VERSION__) &&                                     \
      ((__STDC_VERSION__ - 0) >= 201112L))	/* i.e. supports Universal Character Names */
		*--s = (char) (U'0' + (x & 1));
# else
		*--s = (char) ('0' + (x & 1));
# endif
#endif
#if 0	/* oh wonderful compiler, which is best? */
		x /= 2;
#else
		x >>= 1;
#endif
	} while (x);

	if (zf) {
		while (s > str)
			*--s = '0';
	}

	return s;
}

u_int ilog2msb(uintmax_t);
u_int
ilog2msb(uintmax_t v)
{
	u_int b = 0;

	if (v == 0)
		return ~0U;

#define step(x)	if (v >= ((uintmax_t) 1) << x)		\
			b += x, v >>= x

#ifdef __SIZEOF_LONG_LONG__
/*
 * xxx assume intmax_t is long long, _and_ 128 bits!
 */
# if __SIZEOF_LONG_LONG__ > __SIZEOF_LONG__
	if (sizeof(uintmax_t) > 8)
		step(64);
# endif
#endif
	if (sizeof(uintmax_t) == 8)
		step(32);
	if (sizeof(uintmax_t) >= 4)
		step(16);
	if (sizeof(uintmax_t) >= 2)
		step(8);
	step(4);
	step(2);
	step(1);

#undef step

	return b;
}

/*
 * ilog2() - a wrapper to call builtin if possible...
 */
unsigned int ilog2(uintmax_t);

#ifndef __has_builtin
# define __has_builtin(x) 0  /* for compatibility */
#endif

unsigned int
ilog2(uintmax_t v)
{
	if (v == 0) {
		return ~0U;
	}
#if __has_builtin(__builtin_clz)
	return ((sizeof(uintmax_t) * CHAR_BIT) - 1) ^ (unsigned int) __builtin_clzll(v);
#else
	return ilog2msb(v);
#endif
}

static int
ceil_ilog10(uintmax_t v)
{
	static unsigned long long int const PowersOf10[] =
		{ 1LLU, 10LLU, 100LLU, 1000LLU, 10000LLU, 100000LLU, 1000000LLU,
		  10000000LLU, 100000000LLU, 1000000000LLU, 10000000000LLU,
		  100000000000LLU, 1000000000000LLU, 10000000000000LLU,
		  100000000000000LLU, 1000000000000000LLU, 10000000000000000LLU,
		  100000000000000000LLU, 1000000000000000000LLU,
		  10000000000000000000LLU };
	unsigned int r;

	/*
	 * Given the relationship "log10(v) = log2(v) / log2(10)", we can
	 * transform this to be "log2(v) * (1 / log2(10))", the latter term of
	 * which is approximately 1233/4096, i.e. (1233, followed by a right
	 * shift of 12).
	 *
	 * Add one to ilog2 because it is actually the zero-based index of MSB.
	 */
	r = ((unsigned int) ilog2(v) + 1) * 1233 >> 12;

	/*
	 * Finally, since the result is only an approximation that may be off by
	 * one, the exact value is found by subtracting "v < PowersOf10[r]" from
	 * the result.
	 */
	return (int) r + 1 - (v < PowersOf10[r]); /* "round" up to find ceil(r) */
}

static unsigned int
ilog10(uintmax_t v)
{
	unsigned int r;
	static unsigned long long int const PowersOf10[] =
		{ 1LLU, 10LLU, 100LLU, 1000LLU, 10000LLU, 100000LLU, 1000000LLU,
		  10000000LLU, 100000000LLU, 1000000000LLU, 10000000000LLU,
		  100000000000LLU, 1000000000000LLU, 10000000000000LLU,
		  100000000000000LLU, 1000000000000000LLU, 10000000000000000LLU,
		  100000000000000000LLU, 1000000000000000000LLU,
		  10000000000000000000LLU };

	if (v == 0) {
		return 0;
	}

	/*
	 * Given the relationship "log10(v) = log2(v) / log2(10)", we can
	 * transform this to be "log2(v) * (1 / log2(10))", the latter term of
	 * which is approximately 1233/4096, i.e. (1233, followed by a right
	 * shift of 12).  To do this with unsigned integer math we must
	 * transform again to (log2(v) * 1233) >> 12.
	 */
	r = ((ilog2(v) * 1233) >> 12) + 1;


	/*
	 * Finally, since the result is only an approximation that may be off by
	 * one, the exact value is found by subtracting "v < PowersOf10[r]" from
	 * the result.
	 */
	return r - (v < PowersOf10[r]);
}

static intmax_t
ipow(int base,
     unsigned int expon)
{
	switch (base) {
	case 0:
		return expon == 0 ? 1 : 0;
	case 1:
		return 1;
	}
	switch (expon) {
	case 0:
		return 1;
	case 1:
		return base;
	}
	/* choose from alternate implementations: */
#if 0
	/* brute force */ {
		intmax_t result = 1;

		for (result = base; expon > 1; expon--) {
			result = result * base;
		}

		return result;
	}
#elif 0
	/* recursive exponentiation by squaring... */ {
		intmax_t temp;

		temp = ipow(base, expon / 2);

		if (expon % 2 == 0) {
			return temp * temp;
		} else {
			return (base * temp * temp);
		}
	}
#elif 0
	/* recursive exponentiation by squaring... */ {

		if (expon % 2 == 1) {
			return base * ipow(base, expon - 1);
		} else {
			return isq(ipow(base, expon / 2));
		}
	}
#else
	/* Exponentiation by squaring... */ {
		intmax_t bbase = base;
		intmax_t result = 1;


		while (expon) {
			if (expon & 1)
				result *= bbase;
			expon >>= 1;
			bbase *= bbase;
		}

		return result;
	}
#endif
}

static uintmax_t
uipow(unsigned int base,
      unsigned int expon)
{
	uintmax_t bbase = base;
	uintmax_t result = 1;

	switch (base) {
	case 0:
		return expon == 0 ? 1 : 0;
	case 1:
		return 1;
	}
	switch (expon) {
	case 0:
		return 1;
	case 1:
		return base;
	}

	/* Exponentiation by squaring... */
	while (expon) {
		if (expon & 1)
			result *= bbase;
		expon >>= 1;
		bbase *= bbase;
	}

	return result;
}

#ifdef THE_HARD_WAY
static float
fpow(float x,
     int y)
{
	float res;
	int i;

	res = 1;
	if (y > 0) {
		for (i = 0; i < y; i++) {
			res *= x;
		}
	} else if (y < 0 && x != 0) {
		for (i = 0; i < -y; i++) {
			res /= x;
		}
	}

	return res;
}
#endif

#if 0
static int
safe_base10_strlen(uintmax_t max_val)
{
	char sbuf[BUFSIZ + 1];

	return snprintf(sbuf, (size_t) BUFSIZ, "%ju", max_val) - 1;
}
#endif

#ifdef __SIZEOF_INT128__
static int
print_u128(__uint128_t u128)
{
	int rc;

	if (u128 > UINT64_MAX) {
		__uint128_t	leading  = u128 / uipow(10U, ilog10(UINT64_MAX));
		uint64_t	trailing = (uint64_t) (u128 % uipow(10U, ilog10(UINT64_MAX)));

		rc = print_u128(leading);
		rc += printf("%.*" PRIu64, (int) ilog10(UINT64_MAX), trailing);
	} else {
		uint64_t u64 = (uint64_t) u128;

		rc = printf("%" PRIu64, u64);
	}
	return rc;
}
#endif


/* xxx we should do something with these.... */
#define flt_equals(a, b) (fabsf((a)-(b)) < FLT_EPSILON)
#define dbl_equals(a, b) (fabs((a)-(b)) < DBL_EPSILON)
#define ldbl_equals(a, b) (fabsl((a)-(b)) < LDBL_EPSILON)

/*
 * For an excellent discussion of the limits of floating-point calculations see
 * the following:
 *
 * 	http://www.exploringbinary.com/the-answer-is-one-unless-you-use-floating-point/
 *
 */
static void
report(char *machine)
{
	static long double ldtmpta = 0.0;
	static long double ldtmptb = 0.0;
	static long double ldtmp = 0.0;
	static double dtmpta = 0.0;
	static double dtmptb = 0.0;
	static double dtmp = 0.0;
	static float ftmpta = 0.0;
	static float ftmptb = 0.0;
	static float ftmp = 0.0;

	if (dtmp != 0.0) {
		puts("WTF: !!!  double 0.0 is not zero!");
		exit(1);
	}
	if (ftmp != 0.0) {
		puts("WTF: !!!  float 0.0 is not zero!"); /* xxx "warning: will never be executed"  why? */
		exit(1);
	}

	printf("%s: sizeof intmax = %u.\n", machine, (unsigned int) sizeof(intmax_t));
#ifdef __SIZEOF_INT128__
	printf("%s: sizeof __int128_t = %u.\n", machine, (unsigned int) sizeof(__int128_t));
#endif
	printf("%s: sizeof long double = %u.\n", machine, (unsigned int) sizeof(long double));
	printf("%s: sizeof double = %u.\n", machine, (unsigned int) sizeof(double));
	printf("%s: sizeof float = %u.\n", machine, (unsigned int) sizeof(float));
	putchar('\n');

	printf("0 leading: 10.0 * 10.0 =     %*.*g\t[%%0.0g]\n", 0, 0, 10.0 * 10.0);
	printf("0 leading: 10.0 * 10.0 =     %#*.*g\t[%%#0.0g]\n", 0, 0, 10.0 * 10.0);
	printf("0 leading: 10.0 * 10.0 =     %*.*f\t[%%0.0f]\n", 0, 0, 10.0 * 10.0);
	printf("0 leading: 10.0 * 10.0 =     %#*.*f\t[%%#0.0f]\n", 0, 0, 10.0 * 10.0);
	printf("10 leading: 10.0 * 10.0 =    %*.*g\t[%%10.0g]\n", 10, 0, 10.0 * 10.0);
	printf("10 leading: 10.0 * 10.0 =    %#*.*g\t[%%#10.0g]\n", 10, 0, 10.0 * 10.0);
	printf("10 leading: 10.0 * 10.0 =    %*.*f\t[%%10.0f]\n", 10, 0, 10.0 * 10.0);
	printf("10 leading: 10.0 * 10.0 =    %#*.*f\t[%%#10.0f]\n", 10, 0, 10.0 * 10.0);
	printf("010 leading: 10.0 * 10.0 =   %0*.*g\t[%%10.0g]\n", 10, 0, 10.0 * 10.0);
	printf("010 leading: 10.0 * 10.0 =   %#0*.*g\t[%%#010.0g]\n", 10, 0, 10.0 * 10.0);
	printf("010 leading: 10.0 * 10.0 =   %0*.*f\t[%%010.0f]\n", 10, 0, 10.0 * 10.0);
	printf("010 leading: 10.0 * 10.0 =   %#0*.*f\t[%%#010.0f]\n", 10, 0, 10.0 * 10.0);
	printf("  leading: 10.0 * 10.0 =     %.*g\t[%%.0g]\n", 0, 10.0 * 10.0);
	printf("  leading: 10.0 * 10.0 =     %#.*g\t[%%#.0g]\n", 0, 10.0 * 10.0);
	printf("  leading: 10.0 * 10.0 =     %.*f\t[%%.0f]\n", 0, 10.0 * 10.0);
	printf("  leading: 10.0 * 10.0 =     %#.*f\t[%%#.0f]\n", 0, 10.0 * 10.0);
	printf("010 nopoint: 10.0 * 10.0 =   %0*g\t[%%10g]\n", 10, 10.0 * 10.0);
	printf("010 nopoint: 10.0 * 10.0 =   %#0*g\t[%%#010g]\n", 10, 10.0 * 10.0);
	printf("010 nopoint: 10.0 * 10.0 =   %0*f\t[%%010f]\n", 10, 10.0 * 10.0);
	printf("010 nopoint: 10.0 * 10.0 =   %#0*f\t[%%#010f]\n", 10, 10.0 * 10.0);
	putchar('\n');

	printf("literals: 1.0 / 10.0 * 10.0 =    %#.*g\n", DBL_DIG, 1.0 / 10.0 * 10.0);
	printf("literals: 1.0l / 10.0l * 10.0l = %#.*Lg\n", LDBL_DIG, 1.0l / 10.0l * 10.0l);
	putchar('\n');

	/*
	 * Demonstrate some of the folleys of doing floating point calculations
	 * and then testing for exact equiavlence of the expected decimal value:
	 */
	dtmp = 0.70 + 0.20 + 0.10;
	if (dtmp != 1.00) {
		puts("WTF: !!!  (0.70 + 0.20 + 0.10) is not binary equivalent to 1.0 !");
		printf("0.70 + 0.20 + 0.10 =    %#.*g\n", my_DBL_DECIMAL_DIG, 0.70 + 0.20 + 0.10);
		/* but at least using DBL_DIG it displays as 1.0!!! */
		printf("0.70 + 0.20 + 0.10 =    %#.*g (ok when rounded)\n\n", DBL_DIG, 0.70 + 0.20 + 0.10);
	}
	dtmp = 0.70 + 0.20 + 0.10;
	if (! dbl_equals(dtmp, 1.00)) {
		printf("WTF: !!!  0.70 + 0.20 + 0.10 NOT within DBL_EPSILON of 1.00 =    %#.*g\n\n", my_DBL_DECIMAL_DIG, dtmp);
	}
	/* we can also make it round to 1.0 */
	dtmp = round(dtmp * 100.00) / 100.00;
	if (dtmp != 1.00) {
		/* xxx normally this works, and some compilers (e.g. gcc-4.1.3) even say
		 * "will never be executed" */
		printf("WTF: !!!  round to 1.00 =    %#.*g\n\n", my_DBL_DECIMAL_DIG, dtmp);
	}

	dtmp = 1.0 - 0.9 - 0.1;
	if (dtmp != 0.0) {
		puts("WTF: !!!  literal (1.0 - 0.9 - 0.1) is not binary equivalent to 0.0");
		printf("1.0 - 0.9 - 0.1 =    %#.*g\n", my_DBL_DECIMAL_DIG, 1.0 - 0.9 - 0.1);
		/* xxx and it doesn't even display as 0.0 when rouned!!! */
		printf("1.0 - 0.9 - 0.1 =    %#.*g  XXX not even zero when rounded!\n\n", DBL_DIG, 1.0 - 0.9 - 0.1);
	}
	dtmp = 1.0 - 0.9 - 0.1;
	if (! dbl_equals(dtmp, 0.00)) {
		printf("WTF: !!!  1.0 - 0.9 - 0.1 NOT within DBL_EPSILON of 0.0 =    %#.*g\n\n", my_DBL_DECIMAL_DIG, dtmp);
	}
	dtmp = round(dtmp * 100.00) / 100.00;
	if (dtmp != 0.00) {
		/* normally this works, unless maybe 0.0 != -0.0 */
		printf("WTF: !!!  round to 0.00 =    %#.*g\n\n", my_DBL_DECIMAL_DIG, dtmp);
	}

	printf("literals: 1.0 - 0.9 - 0.1 =    %#.*g\n", my_DBL_DECIMAL_DIG, 1.0 - 0.9 - 0.1);
	printf("literals: 1.0l - 0.9l - 0.1l =  %#.*Lg\n", my_LDBL_DECIMAL_DIG, 1.0l - 0.9l - 0.1l);


	ftmp = 1.0;
	ftmpta = (float) 0.9;
	ftmptb = (float) 0.1;
	ftmp = ftmp - ftmpta - ftmptb;
	printf("float:    1.0 - 0.9 - 0.1 =     %#.*g\n", my_FLT_DECIMAL_DIG, ftmp); /* XXX not 0.0 */

	ftmp = 1.0l;
	ftmpta = 0.9l;			/* xxx loses precision */
	ftmptb = 0.1l;			/* xxx loses precision */
	ftmp = ftmp - ftmpta - ftmptb;
	printf("float:    1.0l - 0.9l - 0.1l =  %#.*g\n", my_FLT_DECIMAL_DIG, ftmp); /* xxx not 0.0 */

	dtmp = 1.0;
	dtmpta = 0.9;
	dtmptb = 0.1;
	dtmp = dtmp - dtmpta - dtmptb;
	printf("double:   1.0 - 0.9 - 0.1 =    %#.*g\n", my_DBL_DECIMAL_DIG, dtmp); /* xxx not 0.0 */

	dtmp = 1.0l;
	dtmpta = 0.9l;			/* xxx loses precision */
	dtmptb = 0.1l;			/* xxx loses precision */
	dtmp = dtmp - dtmpta - dtmptb;
	printf("double:   1.0l - 0.9l - 0.1l = %#.*g\n", my_DBL_DECIMAL_DIG, dtmp); /* xxx not 0.0 */

	ldtmp = 1.0;
	ldtmpta = 0.9;
	ldtmptb = 0.1;
	ldtmp = ldtmp - ldtmpta - ldtmptb;
	printf("l_double: 1.0 - 0.9 - 0.1 =    %#.*Lg\n", my_LDBL_DECIMAL_DIG, ldtmp); /* xxx not 0.0 */

	ldtmp = 1.0l;
	ldtmpta = 0.9l;
	ldtmptb = 0.1l;
	ldtmp = ldtmp - ldtmpta - ldtmptb;
	printf("l_double: 1.0l - 0.9l - 0.1l  = %#.*Lg\n", my_LDBL_DECIMAL_DIG, ldtmp); /* xxx not 0.0 */

	putchar('\n');

	/* xxx passing arg 1 of 'roundf' as 'float' rather than 'double' due to prototype */
	printf("roundf(1.0  - 0.9  - 0.1)  =  %#.*g\n", FLT_DIG, roundf(ftmp));
	/* xxx these will keep their negative sign: */
	printf(" round(1.0  - 0.9  - 0.1)  = %#.*g\n", DBL_DIG, round(dtmp));
#ifdef HAVE_ROUNDL
	printf("roundl(1.0l - 0.9l - 0.1l) =  %#.*Lg\n", LDBL_DIG, roundl(ldtmp));
#endif
	putchar('\n');

	/* but even using our not-quite-zero we do get back to 1.0 exactly */

	ftmpta = (float) 0.9;
	ftmp = ftmp + ftmpta + ftmptb;
	printf("float:    0.0 + 0.9 + 0.1 =     %#.*g\n", FLT_DIG, ftmp);

	dtmpta = 0.9;
	dtmp = dtmp + dtmpta + dtmptb;
	printf("double:   0.0 + 0.9 + 0.1 =     %#.*g\n", DBL_DIG, dtmp);

	ldtmpta = 0.9l;
	ldtmp = ldtmp + ldtmpta + ldtmptb;
	printf("l_double: 0.0l + 0.9l + 0.1l =  %#.*Lg\n", LDBL_DIG, ldtmp);

	putchar('\n');

	/* this should always be OK */
	dtmp = 0.0 + 0.9 + 0.1;
	if (dtmp != 1.0) {
		puts("WTF: !!!  (0.0 + 0.9 + 0.1) is not binary equivalent to 1.0!");
		printf("0.0 + 0.9 + 0.1 =    %#.*g\n", my_DBL_DECIMAL_DIG, 0.0 + 0.9 + 0.1);
		printf("0.0 + 0.9 + 0.1 =    %#.*g (i.e. OK when rounded)\n\n", DBL_DIG, 0.0 + 0.9 + 0.1);
	}
	ftmp = 0.0;
	ftmpta = (float) 0.9;
	ftmptb = (float) 0.1;
	ftmp = ftmp + ftmpta + ftmptb;
	printf("float:    0.0 + 0.9 + 0.1 =     %#.*g\n", FLT_DIG, ftmp);

	dtmp = 0.0;
	dtmpta = 0.9;
	dtmptb = 0.1;
	dtmp = dtmp + dtmpta + dtmptb;
	printf("double:   0.0 + 0.9 + 0.1 =     %#.*g\n", DBL_DIG, dtmp);

	ldtmp = 0.0l;
	ldtmpta = 0.9l;
	ldtmptb = 0.1l;
	ldtmp = ldtmp + ldtmpta + ldtmptb;
	printf("l_double: 0.0l + 0.9l + 0.1l =  %#.*Lg\n", LDBL_DIG, ldtmp);

	/* but now let's try with an inexactly representable decimal value again */
	dtmp = 0.0;
	dtmpta = 0.1;
	dtmptb = 0.11;
	dtmp = dtmp + dtmpta + dtmptb;
	puts("NOTE:  Expected output at max precision for d->b->d:");
	printf("double:   0.0 + 0.1 + 0.11 =     %#.*g [%%#.%dg, i.e. using DBL_DIG]\n", DBL_DIG, dtmp, DBL_DIG);
	puts("NOTE:  Safe output (default precision):");
	printf("double:   0.0 + 0.1 + 0.11 =     %#g [%%#g]\n", dtmp);
	printf("double:   0.0 + 0.1 + 0.11 =     %g [%%g]\n", dtmp);
	printf("double:   0.0 + 0.1 + 0.11 =     %f [%%f]\n", dtmp);
	printf("double:   0.0 + 0.1 + 0.11 =     %e [%%e]\n", dtmp);
	printf("double:   0.0 + 0.1 + 0.11 =     %a [%%a]\n", dtmp);

	putchar('\n');

	ftmp = 1.0l;
	ftmpta = 0.9l;			/* xxx loses precision */
	ftmptb = 0.1l;			/* xxx loses precision */
	ftmp = ftmp - ftmpta - ftmptb;
	printf("float:    1.0l - 0.9l - 0.1l =   %#.*g [%%#.%dg, i.e. using FLT_DECIMAL_DIG]\n", my_FLT_DECIMAL_DIG, dtmp, my_FLT_DECIMAL_DIG);

	putchar('\n');

	puts  ("Nasty:       10.1 - 9.93 =     0.17 (inexact in all widths)");
	putchar('\n');

	ftmp = (float) 10.1;
	ftmpta = (float) 9.93;
	ftmp = ftmp - ftmpta;
	printf("float:       10.1 - 9.93 =     %#.*g\t\t FLT_DECIMAL_DIG\n", my_FLT_DECIMAL_DIG, ftmp);
	printf("float:       10.1 - 9.93 =     %.*f\t\t\t FLT_DIG\n", FLT_DIG, ftmp);
	printf("float:       10.1 - 9.93 =     %.*f\t\t FLT_DIG+1\n", FLT_DIG+1, ftmp);

	dtmp = 10.1;
	dtmpta = 9.93;
	dtmp = dtmp - dtmpta;
	printf("double:      10.1 - 9.93 =     %#.*g\t DBL_DECIMAL_DIG\n", my_DBL_DECIMAL_DIG, dtmp);
	printf("double:      10.1 - 9.93 =     %.*f\t DBL_DIG\n", DBL_DIG, dtmp);
	printf("double:      10.1 - 9.93 =     %.*f\t DBL_DIG+1\n", DBL_DIG+1, dtmp);

	ldtmp = 10.1l;
	ldtmpta = 9.93l;
	ldtmp = ldtmp - ldtmpta;
	printf("l_double:    10.1 - 9.93 =     %#.*Lg\t LDBL_DECIMAL_DIG\n", my_LDBL_DECIMAL_DIG, ldtmp);
	printf("l_double:    10.1 - 9.93 =     %.*Lf\t LDBL_DIG\n", LDBL_DIG, ldtmp);
	printf("l_double:    10.1 - 9.93 =     %.*Lf\t LDBL_DIG+1\n", LDBL_DIG+1, ldtmp);

	putchar('\n');

	puts("Examples:  using '%1.*e'");
	putchar('\n');

	puts("NOTE:  Two too many digits, showing incorrect value, but OK for b->d->b:");
	printf("float:   10.1 - 9.93 =     %1.*e [%%1.%de, i.e. using FLT_DECIMAL_DIG-1]\n", my_FLT_DECIMAL_DIG-1, ftmp, my_FLT_DECIMAL_DIG-1);
	puts("NOTE:  Safe output, d->b->d precision for '%1.*e':");
	printf("float:   10.1 - 9.93 =     %1.*e [%%1.%de, i.e. using FLT_DIG-1]\n", FLT_DIG-1, ftmp, FLT_DIG-1);

	putchar('\n');

	puts("NOTE:  Two too many digits, showing incorrect value, but OK for b->d->b:");
	printf("double:   10.1 - 9.93 =     %1.*e [%%1.%de, i.e. using DBL_DECIMAL_DIG-1]\n", my_DBL_DECIMAL_DIG-1, dtmp, my_DBL_DECIMAL_DIG);
	puts("NOTE:  Safe output, d->b->d precision for '%1.*e':");
	printf("double:   10.1 - 9.93 =     %1.*e [%%1.%de, i.e. using DBL_DIG-1]\n", DBL_DIG-1, dtmp, DBL_DIG-1);

	putchar('\n');

	puts("NOTE:  Three too many digits, showing incorrect value, but OK for b->d->b:");
	printf("l_double: 10.1 - 9.93 =     %1.*Le [%%1.%dLe, i.e. using LDBL_DECIMAL_DIG-1]\n", my_LDBL_DECIMAL_DIG-1, ldtmp, my_LDBL_DECIMAL_DIG);
	puts("NOTE:  Safe output, d->b->d precision for '%1.*e':");
	printf("l_double: 10.1 - 9.93 =     %1.*Le [%%1.%dLe, i.e. using LDBL_DIG-1]\n", LDBL_DIG-1, ldtmp, LDBL_DIG-1);

	putchar('\n');

	puts("Examples:  using '%#.*g'");
	putchar('\n');

	puts("NOTE:  Two too many digits, showing incorrect value, but OK for b->d->b:");
	printf("float:   10.1 - 9.93 =     %#.*g [%%#.%dg, i.e. using FLT_DECIMAL_DIG]\n", my_FLT_DECIMAL_DIG, ftmp, my_FLT_DECIMAL_DIG);
	puts("NOTE:  Safe output, d->b->d precision for '%1.*e':");
	printf("float:   10.1 - 9.93 =     %#.*g [%%#.%dg, i.e. using FLT_DIG]\n", FLT_DIG, ftmp, FLT_DIG);

	putchar('\n');

	puts("NOTE:  Too many digits, showing incorrect value, but OK for b->d->b:");
	printf("double:   10.1 - 9.93 =     %#.*g [%%#.%dg, i.e. using DBL_DECIMAL_DIG]\n", my_DBL_DECIMAL_DIG, dtmp, my_DBL_DECIMAL_DIG);
	puts("NOTE:  Safe output, d->b->d precision for '%#.*g':");
	printf("double:   10.1 - 9.93 =     %#.*g [%%#.%dg, i.e. using DBL_DIG]\n", DBL_DIG, dtmp, DBL_DIG);

	putchar('\n');

	puts("NOTE:  Three too many digits, showing incorrect value, but OK for b->d->b:");
	printf("l_double: 10.1 - 9.93 =     %#.*Lg [%%#.%dLg, i.e. using LDBL_DECIMAL_DIG]\n", my_LDBL_DECIMAL_DIG, ldtmp, my_LDBL_DECIMAL_DIG);
	puts("NOTE:  Safe output, d->b->d precision for '%#.*g':");
	printf("l_double: 10.1 - 9.93 =     %#.*Lg [%%#.%dLg, i.e. using LDBL_DIG]\n", LDBL_DIG, ldtmp, LDBL_DIG);

	putchar('\n');

	puts("Examples:  using '%.*f'");
	putchar('\n');

	puts("NOTE:  Two too many digits, showing incorrect value, but OK for b->d->b:");
	printf("float:   10.1 - 9.93 =     %.*f [%%.%df, i.e. using FLT_DECIMAL_DIG]\n", my_FLT_DECIMAL_DIG, ftmp, my_FLT_DECIMAL_DIG);
	puts("NOTE:  Safe output, d->b->d precision for '%1.*e':");
	printf("float:   10.1 - 9.93 =     %.*f [%%.%df, i.e. using FLT_DIG]\n", FLT_DIG, ftmp, FLT_DIG);

	putchar('\n');

	puts("NOTE:  Two too many digits for %f, showing incorrect value, but OK for b->d->b:");
	printf("double:   10.1 - 9.93 =     %.*f [%%.%df, i.e. using DBL_DECIMAL_DIG]\n", my_DBL_DECIMAL_DIG, dtmp, my_DBL_DECIMAL_DIG);
	puts("NOTE:  Safe output, d->b->d precision for '%.*f':");
	printf("double:   10.1 - 9.93 =     %.*f [%%.%df, i.e. using DBL_DIG]\n", DBL_DIG, dtmp, DBL_DIG);

	putchar('\n');

	puts("NOTE:  Three too many digits for %f, showing incorrect value, but OK for b->d->b:");
	printf("l_double: 10.1 - 9.93 =     %.*Lf [%%.%dLf, i.e. using LDBL_DECIMAL_DIG]\n", my_LDBL_DECIMAL_DIG, ldtmp, my_LDBL_DECIMAL_DIG);
	puts("NOTE:  Safe output, d->b->d precision for '%.*f':");
	printf("l_double: 10.1 - 9.93 =     %.*Lf [%%.%dLf, i.e. using LDBL_DIG]\n", LDBL_DIG, ldtmp, LDBL_DIG);

	putchar('\n');


	dtmpta = 100000.1;
	dtmptb = 2.2;

	dtmptb *= 100.0;
	dtmp = dtmpta / dtmptb;

	/* rounding and truncation.... */

	printf("dtmpta .0 = %4.1f\n", dtmpta);
	printf("dtmptb .0 = %4.1f\n", dtmptb);
	printf("dtmp .0 = %6.1f\n", dtmp);

	printf("dtmptb = %.1f\n", dtmptb);

	putchar('\n');

	dtmptb = 0.5;			/* 0.5 rounds to zero? */

	printf("dtmp = %f\n", dtmp);
	printf("dtmptb = %f\n", dtmptb);
	printf("(int) dtmp = %d\n", (int) dtmp);
	printf("(int) dtmptb = %d\n", (int) dtmptb);
	printf("lrint(dtmp) = %ld\n",  lrint(dtmp));
	printf("lrint(dtmptb) = %ld\n", lrint(dtmptb));

	putchar('\n');

	dtmptb = 0.51;			/* but 0.51 always rounds to 1 */

	printf("dtmp = %f\n", dtmp);
	printf("dtmptb = %f\n", dtmptb);
	printf("(int) dtmp = %d\n", (int) dtmp);
	printf("(int) dtmptb = %d\n", (int) dtmptb);
	printf("lrint(dtmp) = %ld\n",  lrint(dtmp));
	printf("lrint(dtmptb) = %ld\n", lrint(dtmptb));

	printf("dtmp = %f\n", dtmp);
	dtmp++;
	printf("dtmp++ = %f\n", dtmp);

	putchar('\n');

	puts("the right answer to 1.4 * 165 is of course 231\n");

	dtmpta = 1.5 - 0.10;
	dtmptb = 165.0;
	dtmp = dtmpta * dtmptb;

	ldtmpta = 1.5l - 0.10l;
	ldtmptb = 165.0l;
	ldtmp = ldtmpta * ldtmptb;

	printf("1.4 * 165 = %a (%%a)\n", dtmp); /* to demonstrate the problem */
	printf("1.4 * 165 = %.015f (%%.015f)\n", dtmp);
	printf("1.4 * 165 = %.015f round( * 100) / 100\n", round(dtmp * 100) / 100);
	printf("1.4 * 165 = %.015f ceil( * 100) / 100\n", ceil(dtmp * 100) / 100);
	printf("1.4 * 165 = %.015f floor( * 100) / 100\n", floor(dtmp * 100) / 100); /* wrong again */
	printf("1.4 * 165 = %.02f (%%.02f)\n", dtmp);
	/*
	 * n.b.:  Some proper ways to calculate the printf("%f") "precision",
	 * aka the number of significant digits after the decimal point for any
	 * given floating-point number with a magnitude greater than 1.0 and
	 * less than 1.0eDBL_DIG, when printed with "%.*f" are:
	 *
	 * (DBL_DECIMAL_DIG - ceil_ilog10((uintmax_t) llabs(llrint(ceil(dval)))) - 1)
	 * (DBL_DECIMAL_DIG - lrint(ceil(log10(ceil(fabs(dval))))) - 1)
	 * (DBL_DECIMAL_DIG - lrint(floor(log10(ceil(fabs(dval))))))
	 *
	 * I.e. the maximum number of significant decimal digits that can be
	 * represented in an IEEE 754 double precision floating point number,
	 * less the number of decimal digits in the "smallest integer larger
	 * than the example value (the ceiling)", minus one for the digit that
	 * is normally to the left of the decimal point in scientific notation.
	 *
	 * N.B.:  using an IEEE 754 floating point implementation of log10(3f),
	 * means that any integer value larger than 2^53 bits (signed, i.e. plus
	 * a sign bit) (i.e. values with more than 15 base-10 digits, such as
	 * 1000000000000001) may suffer rounding to a representable value and
	 * thus may result in the "wrong" log10() result....
	 *
	 * Note that ceil(log10()) also fails for exact powers of 10 (and zero),
	 * so in some hypothetical scenario if integer modulo plus a comparison
	 * with zero is as fast as addition then the following would correct it:
	 *
	 *	a = fabs(dval);
	 * 	x = ceil(log10(a));
	 * 	if (llrint(a) % 10 == 0)
	 * 		x += 1;
	 *
	 * Here DBL_DECIMAL_DIG is:
	 *
	 *	lrint(ceil(DBL_MANT_DIG * log10((double) FLT_RADIX)) + 1)
	 *
	 * (I.e. the of the number of base-10 digits of significance in a
	 * "double".)
	 *
	 * and where FLT_RADIX is the base of the count of digits given as
	 * DBL_MANT_DIG (always 2 for Standard C since float and double must be
	 * binary32 and binary64 IEEE 754 formats respectively)
	 *
	 * and where DBL_MANT_DIG is the number of digits in the mantissa(*),
	 * and the +1 is because this macro counts the implied binary digit of
	 * precision in IEEE 754 binary floating point formats, but in fact we
	 * have to round the precision down by one FLT_RADIX digit to account
	 * for rounding errors inherent in representing decimal (base-10)
	 * numbers in binary floating point.
	 *
	 * Thus 64-bit floats can exactly represent any integer 53 bits long (or
	 * less), either positive or negative (sign bit is stored separately --
	 * see the declaration of union ieee754_binary64_double_u below).
	 */
	printf("1.4 * 165 = %.*f (%%.%df aka DBL_DECIMAL_DIG[%d] - ceil_ilog10(result)[%d])\n",
	       my_DBL_DECIMAL_DIG - ceil_ilog10((uintmax_t) llabs(llrint(ceil(dtmp)))) - 1,
	       dtmp,
	       my_DBL_DECIMAL_DIG - ceil_ilog10((uintmax_t) llabs(llrint(ceil(dtmp)))) - 1,
	       my_DBL_DECIMAL_DIG,
	       ceil_ilog10((uintmax_t) llabs(llrint(ceil(dtmp)))) - 1);
	printf("1.4 * 165 = %.*f (%%.%ldf aka DBL_DECIMAL_DIG[%d] - floor(log10(ceil(result)))[%g])\n",
	       my_DBL_DECIMAL_DIG - (int) lrint(floor(log10(floor(fabs(dtmp)))))+1,
	       dtmp,
	       my_DBL_DECIMAL_DIG - lrint(floor(log10(ceil(fabs(dtmp))))),
	       my_DBL_DECIMAL_DIG,
	       floor(log10(ceil(fabs(dtmp))))); /* wrong again */

	/*
	 * Normally one might use the built-in human-friendly formatting with
	 * "%g" where the precision (N in '%.Ng') specifies the number of
	 * significant digits to print and so we can just print exactly the
	 * maximum number of significant digits supported by the source binary
	 * format.  Scientific notation will automatically be used whenever the
	 * resulting representation can not be expressed as a normal decimal
	 * number with DBL_DIG signitifant digits, plus the decimal place).
	 *
	 * 	printf("%.*g", DBL_DIG, d)
	 *
	 * Alternately one might present floating point values using full on
	 * scientific notation with "%e".  Again we can also directly print the
	 * maximum number of significant decimal digits supported by the source
	 * binary format, but we have to take into account the one digit before
	 * the decimal place since the precision (N in '%.Ne') specifies the
	 * number of decimal places:
	 *
	 *	printf("%1.*e", DBL_DIG - 1, d)
	 *
	 *
	 * I.e. normally DBL_DIG is the right one to use with "%.*g" (and
	 * DBL_DIG-1 is correct for "%.*e" because of the already present '1.'
	 * as a significant digit).
	 *
	 * However due to the nature of binary floating point vs. decimal, and
	 * the conversion between them, there isn't one precise decimal
	 * presentation of a binary floating point value.
	 *
	 * Further explanation:
	 *
	 *	DECIMAL_DIG is the number of decimal digits you need when
	 *	converting from the largest (widest) floating point type to a
	 *	decimal string and back to ensure that you get back the same
	 *	binary value.
	 *
	 *	This is for the "binary -> decimal -> binary" round-trip.
	 *
	 * (so for less wide:  for a binary "double" this is DBL_DECIMAL_DIG)
	 *
	 * XXX except of course that doesn't round correctly to produce accurate
	 * results in a decimal presentation!  See examples elsewhere herein,
	 * such as "1.4 * 165", thus the use of DBL_DIG with "%.*g", or
	 * DBL_DIG-1 with %1.*e.
	 *
	 *	[[L]DBL|FLT]_DIG is the number of decimal digits that will be
	 *	reliably preserved when converting from decimal to [long] double
	 *	(or float) binary and back (to decimal).
	 *
	 *	This is for the "decimal -> binary floating -> decimal" round-trip.
	 *
	 * For printf("%.*e") the precision (N in "%.Ne") is the number of
	 * digits after the decimal point, so for normal scientific notation
	 * with "%1.*e" we can specify at most the number of significant digits
	 * minus one for the leading digit, or rather the number of decimal
	 * places, i.e. (DBL_DIG-1) for a "double".
	 *
	 * For printf("%.*g") the precision (.*) is the total number of
	 * significant digits to be printed, so at most DBL_DIG for a "double".
	 *
	 * [[above paraphrased quotes from:  <https://stackoverflow.com/a/39707541/816536>]]
	 *
	 * XXX Note that because of the rounding necessary to get correctly
	 * rounded presentable results the decimal values printed for DBL_MAX
	 * and DBL_MIN (etc.) cannot be converted back to binary -- the rounded
	 * value is outside the representable limit!  For pure serialization
	 * (marshalling) and re-reading of binary floating point values one must
	 * to print *DECIMAL_DIG significant digits, only doing rounding to
	 * *_DIG significant digits for final presentation.
	 *
	 * XXX ToDo XXX work out a valid rounding for *_MIN and *_MAX values
	 * such that they can still be printed with DBL_DIG _and_ survive
	 * re-reading (though of course to their new rounded value, i.e. it
	 * would be OK to loose their minmax quality)
	 *
	 * XXX ToDo XXX also/alternately work out what the *_DEC_{MIN,MAX}
	 * values are (i.e. the minmax which don't suffer rounding "error" when
	 * doing the decimal -> binary -> decimal round-trip.
	 */
	putchar('\n');
	printf("1.4 * 165 = %1.*e (%%1.%de aka DBL_DIG[%d] - 1)\n",
	       DBL_DIG - 1, dtmp, DBL_DIG - 1, DBL_DIG);
	printf("1.4 * 165 = %1.*e (%%1.%de aka DBL_DIG)\n",
	       DBL_DIG, dtmp, DBL_DIG);
	printf("1.4 * 165 = %1.*e (%%1.%de aka DBL_DIG + 1)\n",
	       DBL_DIG + 1, dtmp, DBL_DIG + 1); /* wrong again */

	putchar('\n');
	printf("1.4 * 165 = %.*g (%%.%dg aka DBL_DIG)\n",
	       DBL_DIG, dtmp, DBL_DIG);
	printf("1.4 * 165 = %.*g (%%.%dg aka DBL_DIG + 1)\n",
	       DBL_DIG + 1, dtmp, DBL_DIG + 1);
	printf("1.4 * 165 = %.*g (%%.%dg aka DBL_DECIMAL_DIG)\n",
	       my_DBL_DECIMAL_DIG, dtmp, my_DBL_DECIMAL_DIG); /* wrong again */

	putchar('\n');

	/* N.B.:  not inexact for long double */

	printf("1.4l * 165l = %La (%%La)\n", ldtmp);
	printf("1.4l * 165l = %.015Lf (%%.015Lf)\n", ldtmp);
	printf("1.4l * 165l = %.015Lf roundl( * 100) / 100\n", roundl(ldtmp * 100) / 100);
	printf("1.4l * 165l = %.015Lf ceill( * 100) / 100\n", ceill(ldtmp * 100) / 100);
	printf("1.4l * 165l = %.015Lf floorl( * 100) / 100\n", floorl(ldtmp * 100) / 100);
	printf("1.4l * 165l = %.02Lf (%%.02Lf)\n", ldtmp);

	putchar('\n');
	printf("1.4l * 165l = %.*Lg (%%.%dg aka LDBL_DIG)\n",
	       LDBL_DIG, ldtmp, LDBL_DIG);
	printf("1.4l * 165l = %.*Lg (%%.%dg aka LDBL_DIG + 1)\n",
	       LDBL_DIG + 1, ldtmp, LDBL_DIG + 1);
	printf("1.4l * 165l = %.*Lg (%%.%dg aka LDBL_DECIMAL_DIG)\n",
	       my_LDBL_DECIMAL_DIG, ldtmp, my_LDBL_DECIMAL_DIG);

	putchar('\n');

	/*
	 * this is an example of how it goes wrong for values with a magnitude
	 * greater than 1.0eDBL_DIG
	 *
	 * xxx and of course there is no way to fix this using printf() alone
	 * for %f as the "precision" is always just "the number of digits to
	 * appear after the decimal-point"
	 *
	 * see print_decimal() for how to deal with this
	 */
	puts("These show incorrect rounding and too many significant (non-zero) digits:");
	dtmp = 0.70 + 0.20 + 0.10;
	dtmp *= pow(10, 20.0);
	printf(" %f (%%f) !=\n 100000000000000000000\n", dtmp);
	dtmp = 1.4 * 165;
	dtmp *= pow(10, 20.0);
	printf(" %f (%%f) !=\n 23100000000000000000000\n", dtmp);
	dtmp = 1234567890123456789.0;
	printf(" %f (%%f) !=\n 1234567890123456789.0\n", dtmp);
	dtmp = 12345678901234567890123456789.0;
	printf(" %f (%%f) !=\n 12345678901234567890123456789.0\n", dtmp);
	printf("DBL_MAX = %f (%%f)\n", DBL_MAX);

	putchar('\n');

	/*
	 * For numbers with a magnitude smaller than 1.0 we must add the number
	 * of leading zeros after the decimal and before the first non-zero
	 * digit to DBL_DECIMAL_DIG.  For a number like OneSeventh/1000000.0,
	 * one would need DBL_DECIMAL_DIG + 6 to see all the significant digits.
	 *
	 * In this example all we see are the first few insignificant zeros.
	 *
	 * see print_decimal() for the general solution
	 */
	puts("This never shows enough (or even any) of the significant digits:");
	printf(" DBL_MIN = % f (%% f)\n", DBL_MIN);
	printf("-DBL_MIN = % f (%% f)\n", -DBL_MIN);
	puts("However if we add in the number of leading zeros it is OK:");
	printf(" DBL_MIN = % .*f (%% .%df)\n",
	       DBL_DIG - 1 + abs((int) lrint(floor(log10(fabs(DBL_MIN))))),
	       DBL_MIN,
	       DBL_DIG - 1 + abs((int) lrint(floor(log10(fabs(DBL_MIN))))));
	printf("-DBL_MIN = % .*f (%% .%df)\n",
	       DBL_DIG - 1 + abs((int) lrint(floor(log10(fabs(-DBL_MIN))))),
	       -DBL_MIN,
	       DBL_DIG - 1 + abs((int) lrint(floor(log10(fabs(-DBL_MIN))))));

	putchar('\n');

	return;
}

double
mult_d(double,
       double)	__attribute__((noinline));

double
mult_d(double x,
       double y)
{
	return x * y;
}

long double
quot_ld(long double,
        long double)	__attribute__((noinline));

long double
quot_ld(long double num,
        long double den)
{
	return num / den;
}

double
quot_d(double,
       double)	__attribute__((noinline));

double
quot_d(double num,
       double den)
{
	return num / den;
}

float
quot_f(float,
       float)	__attribute__((noinline));

float
quot_f(float num,
       float den)
{
	return num / den;
}

/*
 * contrived example from:
 *
 * 	http://www.exploringbinary.com/the-answer-is-one-unless-you-use-floating-point/
 *
 * 2.1 -> 1.0
 * 553774558711019983333.9 -> -65536
 *
 * Also note from WECSSKAFPA:
 *
 * Due to roundoff errors, the associative laws of algebra do not necessarily
 * hold for floating-point numbers.  For example, the expression (x+y)+z has a
 * totally different answer than x+(y+z) when x = 1030, y = -1030 and z = 1 (it
 * is 1 in the former case, 0 in the latter).
 */
double
give_me_one(double)	__attribute__((noinline));
double
give_me_one(double a)
{
	double b;
	double c;

	b = (10 * a) - 10;
	c = a - (0.1 * b);

	return c;
}


/*
 * definitions for the internal representation of IEEE 754 binary 32-bit
 * floating point
 *
 * Note:  Apparently "Significand" is now the preferred term for the fraction
 * part of a floating-point number.
 *
 * In mathematics a "mantissa" is defined as "The decimal part of a logarithm
 * base 10, as distinguished from the integral part", and of course floating
 * point numbers are not logarithms nor logarithmic in nature.
 *
 * However the Free On-line Dictionary of Computing defines "mantissa" as:
 *
 *   The part of a {floating point} number which, when multiplied by its {radix}
 *   raised to the power of its {exponent}, gives its value.  The mantissa may
 *   include the number's sign or this may be considered to be a separate part.
 *
 * It is said that "significands" (in scientific notation and floating point)
 * are log-linear (if the number increases by a factor of 1.2, the significand
 * increases by a factor of 1.2, unless an exponent threshold is crossed), while
 * mantissas are logarithmic.
 *
 * Although the word "mantissa" does not appear in IEEE 754, since the manifest
 * #defines for this part of the floating point format uses the abbreviation
 * "MANT", we'll stick with "mantissa" in the coding arena.
 *
 * Besides, "significand" is only a "recent" word in most English dictionaries,
 * and is not even in many computer spelling dictionaries.  The OED puts its
 * earlies known use in the 1940's, and suggests it is a "borrowing from Latin,
 * with an English element".  Furthermore the OED gives the following definition
 * of "manitissa" for "computing":
 *
 *	the part of a floating-point number that represents the significant
 *	digits of that number, and that is multiplied by the base raised to the
 *	exponent to give the actual value of the number.
 *
 * Apparently Johann Carl Friedrich Gauss, presumably sometime before his death
 * in 1855, suggested using "mantissa" for the fractional part of all decimals.
 * https://mathshistory.st-andrews.ac.uk/Miller/mathword/m/
 *
 * In an early (1946? 1963?) description of floating point John von Neumann used
 * the word mantissa (to describe the value after the decimal point) [[though in
 * this writing von Neumann was actually aruging against implementation of
 * floating point]]:
 *
 *	a so-called "floating decimal point".  This is a mechanism for
 *	expressing each word as a characteristic and a mantissa
 *  https://www.cs.princeton.edu/courses/archive/fall10/cos375/Burks.pdf
 *
 * The book "Scientific Computing: an Introductory Survey" indicates that
 * mantissa and significand are synonyms and then goes on to use the term
 * mantissa from that point.
 *
 * Kahan and Knuth discourage use of "mantissa", but they are both pedants to
 * the extreme.
 *
 * Some further history of floating point in digital computing:
 *
 * The first digital computer (according to some), the Z3, built by Konrad Zuse
 * in 1941 used floating point representation.  (decimal?)
 *
 * In 1949 the Bell Labs "Model V" electro-mechanical computer used decimal
 * floating-point numbers.
 *
 * In 1950, the National Physical Laboratory, UK's "Pilot ACE" had binary
 * floating-point (eventually), implemented in software.
 *
 * In 1954 the IBM 704 had single-precision floating-point numbers with a
 * magnitude sign, an 8-bit excess-128 exponent, and a 27-bit fraction (no
 * hidden bit).
 *
 * In 1963 the IBM 7094 introduced double-precision floating-point numbers with
 * a magnitude sign, an eight-bit excess-128 exponent, and a 54-bit magnitude.
 */
#if (FLT_RADIX != 2) /* only FLT_RADIX is guaranteed to be a constant expr. */
# error "ERROR: FLT_RADIX != 2 !!!"
#endif
#define calc_SNG_EXPBITS	(32 - FLT_MANT_DIG)
#ifdef SNG_EXPBITS
# define my_SNG_EXPBITS		SNG_EXPBITS
#else
# define my_SNG_EXPBITS		calc_SNG_EXPBITS
#endif
#define calc_SNG_FRACBITS	(FLT_MANT_DIG - 1)
#ifdef SNG_FRACBITS
# define my_SNG_FRACBITS	SNG_FRACBITS
#else
# define my_SNG_FRACBITS	calc_SNG_FRACBITS
#endif
typedef union ieee754_binary32_float_u {
	uint32_t i;
	float f;
	struct ieee_float_internals {
		/* Bitfields for exploration. */
#ifdef _BIT_FIELDS_LTOH
		uint32_t mantissa : my_SNG_FRACBITS;
		uint32_t exponent : my_SNG_EXPBITS;
		uint32_t sign : 1;
#else
		uint32_t sign : 1;
		uint32_t exponent : my_SNG_EXPBITS;
		uint32_t mantissa : my_SNG_FRACBITS;
#endif
	} fbits;
} my_float_t;

/*
 * Portable extraction of components without a union bitfield overlay...
 */
bool
f_sign(my_float_t);

bool
f_sign(my_float_t f)
{
	/*
	 * xxx could also use a mask of
	 *
	 * 	~(~0ULL >> ((sizeof(f.i) * CHAR_BIT) - 1))
	 *
	 * which the compiler should be able to turn into constant as it is a
	 * constant expression....
	 */
	return (f.i >> 31) != 0;
}

int32_t
f_mantissa(my_float_t);

int32_t
f_mantissa(my_float_t f)
{
	/* turn off the sign and exponent bits, everything but the mantissa */
	return f.i & (~0U >> (1 + my_SNG_EXPBITS));
}

int32_t
f_exponent(my_float_t);

int32_t
f_exponent(my_float_t f)
{
	/* turn off the sign bit and shift exponent over the mantissa */
	return (f.i & (~0U >> 1)) >> my_SNG_FRACBITS;
}

void
print_float_internals(float);

#define calc_SNG_EXP_BIAS	(FLT_MAX_EXP - 1)
#ifdef SNG_EXP_BIAS
# define my_SNG_EXP_BIAS	SNG_EXP_BIAS
#else
# define my_SNG_EXP_BIAS	calc_SNG_EXP_BIAS
#endif

void
print_float_internals(float v)
{
	my_float_t f;

	f.f = v;
	printf("hex: 0x%016jx, sign=%u, exp=0x%03x, mant=0x%014jx\n"
	       "log10() = %.3f, exp = %ud - bias(%d) = %dd, mant = %jud\n",
	       (uintmax_t) f.i,
	       f.fbits.sign,
	       f.fbits.exponent,
	       (uintmax_t) f.fbits.mantissa,
	       floor(log10(fabs(v))),
	       f.fbits.exponent, my_SNG_EXP_BIAS, f.fbits.exponent - my_SNG_EXP_BIAS,
	       (uintmax_t) f.fbits.mantissa);
	printf("base-2: %s\n", binary_fmt((uintmax_t) f.i, true));

	return;
}

float
re_encode_float(uint32_t ieee754_bin32);

float
re_encode_float(uint32_t ieee754_bin32)
{
	my_float_t fu;

	fu.i = ieee754_bin32;

	return fu.f;
}


/*
 * fancy formatted output for IEEE 754 float
 */
void fancy_ieee754_float(float f);

void fancy_ieee754_float(float f)
{
	my_float_t fu;
	int i = sizeof(f) * CHAR_BIT;

	fu.f = f;
	putchar(' ');
	while (i--) {
		printf("%d ", (fu.i >> i) & 0x1);
	}
	putchar('\n');
	printf("|-|- - - - - - - -|- - - - - - - - - - - - - - - - - - - - - - -|\n");
	printf("|s|    exponet    |                  mantissa                   |\n\n");
}


/*
 * definitions of internal representation for IEEE 754 binary 64-bit floating point
 *
 * note on NetBSD there's <machine/ieee.h> (which includes <sys/ieee754.h> and
 * it has:
 *
 *	#define	DBL_EXPBITS	11
 *	#define	DBL_FRACHBITS	20
 *	#define	DBL_FRACLBITS	32
 *	#define	DBL_FRACBITS	(DBL_FRACHBITS + DBL_FRACLBITS)
 *
 * NetBSD's "struct ieee_double" also uses two "unsigned int" fields, thus the
 * separate "*HBITS" and "*LBITS" definitions.
 *
 * XXX this uses uint64_t as a bitfield and this may not be portable, though
 * NetBSD also uses uint64_t in its definition of BINARY128 internals.
 *
 * XXX this also mixes unit32_t and uint64_t fractional bitfields in the same
 * struct, and that may also not be portable.
 */
#define calc_DBL_EXPBITS	(64 - DBL_MANT_DIG)
#ifdef DBL_EXPBITS
# define my_DBL_EXPBITS		DBL_EXPBITS
#else
# define my_DBL_EXPBITS		calc_DBL_EXPBITS
#endif
#define calc_DBL_FRACBITS	(DBL_MANT_DIG - 1)
#ifdef DBL_FRACBITS
# define my_DBL_FRACBITS	DBL_FRACBITS
#else
# define my_DBL_FRACBITS	calc_DBL_FRACBITS
#endif
typedef union ieee754_binary64_double_u {
	int64_t i;
	double d;
	struct ieee_double_internals {
		/* Bitfields for exploration. */
#ifdef _BIT_FIELDS_LTOH
		uint64_t mantissa : my_DBL_FRACBITS;
		uint32_t exponent : my_DBL_EXPBITS;
		uint32_t sign : 1;
#else
		uint32_t sign : 1;
		uint32_t exponent : my_DBL_EXPBITS;
		uint64_t mantissa : my_DBL_FRACBITS;
#endif
	} dbits;
} my_double_t;


#define calc_DBL_EXP_BIAS	(DBL_MAX_EXP - 1)
#ifdef DBL_EXP_BIAS
# define my_DBL_EXP_BIAS	DBL_EXP_BIAS
#else
# define my_DBL_EXP_BIAS	calc_DBL_EXP_BIAS
#endif

/*
 * fancy formatted output for IEEE 754 double
 */
void fancy_ieee754_double(double d);

void fancy_ieee754_double(double d)
{
	my_double_t du;
	int i = sizeof(d) * CHAR_BIT;

	du.d = d;
	putchar(' ');
	while (i--) {
		printf("%u", (u_int) ((du.i >> i) & 0x1));
		switch (i) {
		case DBL_MANT_DIG - 1:
		case (sizeof(d) * CHAR_BIT) - 1:
			putchar(' ');
			break;
		}
	}
	putchar('\n');
	printf("|-|-----------|----------------------------------------------------|\n");
	printf("|s| exponent  |                      mantissa                      |\n\n");
}

void
print_double_internals(double);

void
print_double_internals(double v)
{
	my_double_t du;

	du.d = v;

	printf("hex: 0x%016jx, exp = 0x%03x, mant = 0x%014jx\n"
	       "dec: exp = %ud - bias(%d) = %dd, mant = %jud\n"
	       "sign=%u, log10() = % .3f\n",

	       (uintmax_t) du.i,
	       du.dbits.exponent,
	       (uintmax_t) du.dbits.mantissa,

	       du.dbits.exponent,
	       my_DBL_EXP_BIAS,
	       du.dbits.exponent - my_DBL_EXP_BIAS,
	       (uintmax_t) du.dbits.mantissa,

	       du.dbits.sign,
	       floor(log10(fabs(v))));

	if (ilogb(v) != du.dbits.exponent - my_DBL_EXP_BIAS) {
		printf("ilogb(): %d", ilogb(v));
		if (isinf(v) && ilogb(v) != INT_MAX) {
			/* NetBSD gets this wrong!!! */
			printf(" XXX should be: %d\n", INT_MAX);
		} else if ((isnan(v) || v == 0.0) && ilogb(v) != INT_MIN) {
			printf(" XXX should be: %d\n", INT_MIN);
		} else {
			putchar('\n');
		}
	}
	printf("base-2: %s\n", binary_fmt((uintmax_t) du.i, true));
	fancy_ieee754_double(du.d);

	return;
}

#define calc_LDBL_EXP_BIAS	(LDBL_MAX_EXP - 1)
#ifdef LDBL_EXP_BIAS
# define my_LDBL_EXP_BIAS	LDBL_EXP_BIAS
#else
# define my_LDBL_EXP_BIAS	calc_LDBL_EXP_BIAS
#endif

void
print_decimal(double);

/*
 * Note:  As presented this demo code prints a whole line including information
 * about how the form was arrived with, as well as in certain cases a couple of
 * interesting details about the number, such as the number of decimal places,
 * and possibly the magnitude of the value and the number of significant digits.
 */
void
print_decimal(double d)
{
	size_t sigdig;
	double flintmax;

	/*
	 * If we really want to see a plain decimal presentation with all of
	 * the possible significant digits of precision for a floating point
	 * number, then we must calculate the correct number of decimal places
	 * to show with "%.*f" as follows.
	 *
	 * Note:  "flintmax" here refers to the largest consecutive integer that
	 * can be safely stored in a floating point variable without losing
	 * precision.
	 *
	 * See also:  http://www.cs.tut.fi/~jkorpela/c/eng.html
	 * (regarding engineering notation, i.e. scaling so that the mantissa is
	 * between 0.1 and 1000.0 and the exponent is a multiple of three)
	 */
#ifdef PRINT_ROUND_TRIP_SAFE
# ifdef DBL_DIG		/* xxx elsewhere we assume this is always defined! */
	sigdig = DBL_DIG;
# else
	/* xxx this is just to show orthogonally how to calculate this */
#  if FLT_RADIX == 2
	sigdig = ceil_ilog10(1LLU << (DBL_MANT_DIG - 1));
#  else
	sigdig = ceil_ilog10(uipow(FLT_RADIX, DBL_MANT_DIG - 1));
#  endif
# endif
#else  /* not PRINT_ROUND_TRIP_SAFE */
	sigdig = my_DBL_DECIMAL_DIG;
#endif

#if 0
	printf("sigdig = %zu\n", sigdig);
#endif

	flintmax = pow((double) FLT_RADIX, (double) DBL_MANT_DIG); /* xxx use uipow() */
	if (d == 0.0) {
		/* fake a "zero" with the right number of significant digits */
		printf("z =  %.*s [%d decimal places]\n",
		       (int) sigdig + 1,
		       "0.000000000000000000000", /* 21 decimal places, assuming double is 64 bits or less */
		       (int) sigdig - 1);
	} else if (fabs(d) >= 0.1 &&
	           fabs(d) <= flintmax) {
		int dplaces;

		dplaces = (int) (sigdig - (size_t) lrint(floor(log10(ceil(fabs(d))))) + 1);
		if (dplaces < 0) {
			/* XXX this is likely never less than -1 */
			/*
			 * XXX the last digit is not significant!!! XXX
			 *
			 * This should also be printed with sprintf() and edited...
			 */
			printf("R = % .0f [%d too many significant digits!!!, zero decimal places]\n", d, abs(dplaces));
		} else if (dplaces == 0) {
			/*
			 * The decimal fraction here is not significant and
			 * should always be zero (XXX I've never seen this -- it
			 * should probably only occur for d==0.0 ???)
			 */
			printf("R = % .0f [zero decimal places]\n", d);
		} else {
			if (fabs(d) == 1.0) {
				/*
				 * This is a special case where the calculation
				 * is off by one because log10(1.0) is 0, but
				 * we still have the leading '1' whole digit to
				 * count as a significant digit.
				 */
#if 0
				printf("ceil(1.0) = %f, log10(ceil(1.0)) = %f, floor(log10(ceil(1.0)))+1 = %f, dplaces--\n",
				       ceil(fabs(d)), log10(ceil(fabs(d))), floor(log10(ceil(fabs(d))))+1);
#endif
				dplaces--;
			}
			/* this is really the "useful" range of %f */
			printf("r = % .*f [%d decimal places]\n", dplaces, d, dplaces);
		}
	} else {
		if (fabs(d) < 1.0) {
			int lz;
			int dplaces;

			lz = abs((int) lrint(floor(log10(fabs(d)))));
			/* i.e. add # of leading zeros to the precision */
			dplaces = (int) sigdig - 1 + lz;
			printf("f = % .*f [%d decimal places, %d leading zeros]\n", dplaces, d, dplaces, lz);
#if 0
			printf("lz = %d, sigdig = %ju, dplaces = %d, dbl = %g\n", lz, (uintmax_t) sigdig, dplaces, d);
#endif
		} else {		/* d > flintmax */
			size_t n;
			size_t i;
			char *df;

			/*
			 * hmmmm...  the easy way to suppress the "invalid",
			 * i.e. non-significant digits is to do a string
			 * replacement of all digits after the first
			 * DBL_DECIMAL_DIG to convert them to zeros, and to
			 * round the least significant digit.
			 */
			df = malloc((size_t) 1);
			n = (size_t) snprintf(df, (size_t) 1, "% .1f", d);
			n++;		/* for the NUL */
			df = realloc(df, n);
			(void) snprintf(df, n, "% .1f", d);
			if ((n - 2) > sigdig) {
				/*
				 * XXX rounding the integer part here is "hard"
				 * -- we would have to convert the digits up to
				 * this point back into a binary format and
				 * round that value appropriately in order to
				 * do it correctly.
				 */
				if (df[sigdig] >= '5' && df[sigdig] <= '9') {
					if (df[sigdig - 1] == '9') {
						/*
						 * xxx fixing this is left as
						 * an exercise to the reader!
						 */
						printf("F =  *** failed to round integer part at the least significant digit!!! ***\n");
						free(df);
						return;
					} else {
						df[sigdig - 1]++;
					}
				}
				for (i = sigdig; df[i] != '.'; i++) {
					df[i] = '0';
				}
			} else {
				i = n - 1; /* less the NUL */
				if (isnan(d) || isinf(d)) {
					sigdig = 0; /* "nan" or "inf" */
				}
			}
			printf("F = %.*s. [0 decimal places, %lu digits, %lu digits significant]\n",
			       (int) i, df, (unsigned long int) i, (unsigned long int) sigdig);
			free(df);
		}
	}

	return;
}

void
print_drep(double);

void
print_drep(double d)
{
	/*
	 * For printf("%e") the precision (.*) is the number of digits after the
	 * decimal point, so at most the number of significant digits - 1 for
	 * the leading digit, i.e. (DBL_DIG-1) for a "double".
	 */
	printf("e = % 1.*e [%d decimal places for d->b->d]\n", DBL_DIG - 1, d, DBL_DIG - 1);
	/*
	 * For printf("%g") the precision (.*) is the total number of
	 * significant digits to be printed, so at most DBL_DIG for a "double"
	 */
	printf("g = % #.*g [%d significant digits for d->b->d]\n", DBL_DIG, d, DBL_DIG);
	/* and for b->d->b */
	printf("e = % 1.*e [%d decimal places for b->d->b]\n", my_DBL_DECIMAL_DIG - 1, d, my_DBL_DECIMAL_DIG - 1);
	printf("g = % #.*g [%d significant digits for b->d->b]\n", my_DBL_DECIMAL_DIG, d, my_DBL_DECIMAL_DIG);
	/*  */
	printf("a = % a\n", d);
	print_decimal(d);
	print_double_internals(d);

	putchar('\n');

	return;
}

/* n.b.: FP_RZ et al might be enum identifiers and actually have the type fp_rnd_t */
#if defined(HAVE_FPGETROUND)
static const char *
fpround2nm(unsigned int r)
{
	switch (r) {
	case FP_RZ:
		return __STRING(FP_RZ);
	case FP_RM:
		return __STRING(FP_RM);
	case FP_RN:
		return __STRING(FP_RN);
	case FP_RP:
		return __STRING(FP_RP);
	default:
		return "[[invalid]]";
	}
}
#endif

/*
 * see: https://stackoverflow.com/a/17035583
 * and: http://stereopsis.com/sree/fpu2006.html
 *
 * This should be comparable to llrint(3), though that will likely use the
 * FISTPL instruction on intel and compatible processors.
 *
 * XXX untested, so far
 */
int64_t cast_d2int64(double);

int64_t
cast_d2int64(double input)		/* xxx must be IEEE 754 BINARY64 */
{
	union DCast {
		double d;
		int64_t l;
	};
	volatile union DCast magic;

	magic.d = input + 6755399441055744.0; /* 2^51 + 2^52 or 2^52*1.5 */
	magic.l <<= 13;
	magic.l >>= 13;

	return magic.l;
}

#if (! defined(HAVE_IEEEFP_H) &&                                        \
     (defined(__STDC_VERSION__) &&                                      \
      ((__STDC_VERSION__ - 0) >= 199901L)) ||                           \
     defined(HAVE_FEGETROUND))
static const char *
feround2nm(int r)
{
	switch (r) {
	case FE_TONEAREST:
		return __STRING(FE_TONEAREST);
	case FE_DOWNWARD:
		return __STRING(FE_DOWNWARD);
	case FE_UPWARD:
		return __STRING(FE_UPWARD);
	case FE_TOWARDZERO:
		return __STRING(FE_TOWARDZERO);
	default:
		return "[[invalid]]";
	}
}
#endif

static double
foo(double v)
{
        double y = v * v;

        return (y / v);
}

static void
representations()			/* xxx clang is getting mouthy */
{
	long double ldq;
	double dq;
	float fq;


	/*  */ {
		volatile double x = 2597.525;
		volatile double y = 5000.525; /* stored as 5000.524999999999636202119290828704833984375 */

		printf("%.2f\n"		/* 2597.53 */
		       "%.2f\n",	/* 5000.52 -- i.e. rounds to nearest even */
		       x, y);
	}
	putchar('\n');
	printf("foo(1E308) = %.*g\n", DBL_DIG, foo(1E308)); /* xxx may be different with/(out) optimization */
	printf("56789.012345 / 1111111111 = %.*g\n", DBL_DIG, quot_d(56789.012345, 1111111111.0));
	putchar('\n');

	printf("0.99 * 0.99 = %.50f (%%.50f)\n", mult_d(0.99, 0.99));
	printf("0.99 * 0.99 = %.*f (rounded with %%.%df [max precision for b->d->b round-trip precision])\n", my_DBL_DECIMAL_DIG, mult_d(0.99, 0.99), my_DBL_DECIMAL_DIG);
	printf("0.99 * 0.99 = %.*f (rounded with %%.%df [d->b->d round-trip precision])\n", DBL_DIG, mult_d(0.99, 0.99), DBL_DIG);
	printf("0.99 * 0.99 = %.04f (rounded with %%.04f)\n", mult_d(0.99, 0.99));
	putchar('\n');

	printf("1.40 (%19.*g) * 165. = %1.*g (%%.%dg [DBL_DECIMAL_DIG])\n", my_DBL_DECIMAL_DIG, 1.5 - 0.10, my_DBL_DECIMAL_DIG, mult_d(1.5 - 0.10, 165.0),  my_DBL_DECIMAL_DIG);
	printf("1.40 (%19.16f) * 165. = %.16f (rounded with %%.16f)\n", 1.50 - 0.10, mult_d(1.50 - 0.10, 165.0));
	printf("1.40 (%19.15f) * 165. = %.15f (rounded with %%.15f)\n", 1.50 - 0.10, mult_d(1.50 - 0.10, 165.0));
	printf("1.40 (%19.14f) * 165. = %.14f (rounded with %%.14f)\n", 1.50 - 0.10, mult_d(1.50 - 0.10, 165.0));
	printf("1.40 (%19.13f) * 165. = %.13f (rounded with %%.13f)\n", 1.50 - 0.10, mult_d(1.50 - 0.10, 165.0));
	printf("1.40 (%19.12f) * 165. = %.12f (rounded with %%.12f)\n", 1.50 - 0.10, mult_d(1.50 - 0.10, 165.0));
	printf("1.40 (%19.04f) * 165. = %.04f (rounded with %%.04f)\n", 1.50 - 0.10, mult_d(1.50 - 0.10, 165.0));
	printf("1.40 (%19.*g) * 165. = %1.*g (%%.%dg [DBL_DECIMAL_DIG])\n", my_DBL_DECIMAL_DIG, 1.40, my_DBL_DECIMAL_DIG, mult_d(1.40, 165.0),  my_DBL_DECIMAL_DIG);
	printf("1.40 (%19.16f) * 165. = %.16f (rounded with %%.16f)\n", 1.40, mult_d(1.40, 165.0));
	printf("1.40 (%19.15f) * 165. = %.15f (rounded with %%.15f)\n", 1.40, mult_d(1.40, 165.0));
	printf("1.40 (%19.14f) * 165. = %.14f (rounded with %%.14f)\n", 1.40, mult_d(1.40, 165.0));
	printf("1.40 (%19.13f) * 165. = %.13f (rounded with %%.13f)\n", 1.40, mult_d(1.40, 165.0));
	printf("1.40 (%19.12f) * 165. = %.12f (rounded with %%.12f)\n", 1.40, mult_d(1.40, 165.0));
	printf("1.40 (%19.04f) * 165. = %.04f (rounded with %%.04f)\n", 1.40, mult_d(1.40, 165.0));
	printf("1.40 (%19.016f) * 165. = %015.16f (rnded w/ %%015.16f)\n", 1.40, mult_d(1.40, 165.0));
	printf("1.40 (%19.016f) * 165. = %019.16f (rnded w/ %%019.16f)\n", 1.40, mult_d(1.40, 165.0));
	printf("1.40 (%19.016f) * 165. = %020.16f (rnded w/ %%020.16f)\n", 1.40, mult_d(1.40, 165.0));
	printf("1.40 (%19.016f) * 165. = %021.16f (rnded w/ %%021.16f)\n", 1.40, mult_d(1.40, 165.0));
	printf("1.40 (%19f) * 165. = %f (rounded with %%f)\n", 1.40, mult_d(1.40, 165.0));
	printf("1.40 (%19g) * 165. = %g (rounded with %%g)\n", 1.40, mult_d(1.40, 165.0));
	printf("1.40 (%#19g) * 165. = %#g (rounded with %%#g)\n", 1.40, mult_d(1.40, 165.0));
	printf("1.40 (%19.*g) * 165. = %.*g (rounded with %%.%dg [DBL_DIG])\n", DBL_DIG, 1.40, DBL_DIG, mult_d(1.40, 165.0), DBL_DIG);
	printf("1.40 (%19.*g) * 165. = %1.*g (rounded with %%1.%dg)\n", DBL_DIG, 1.40, DBL_DIG, mult_d(1.40, 165.0), DBL_DIG);
	printf("1.40 (%019.*g) * 165. = %01.*g (rounded with %%01.%dg)\n", DBL_DIG, 1.40, DBL_DIG, mult_d(1.40, 165.0), DBL_DIG);
	printf("1.40 (%#19.*g) * 165. = %#1.*g (rounded with %%#1.%dg)\n", DBL_DIG, 1.40, DBL_DIG, mult_d(1.40, 165.0), DBL_DIG);
	printf("1.40 (%#019.*g) * 165. = %#01.*g (rounded with %%#01.%dg)\n", DBL_DIG, 1.40, DBL_DIG, mult_d(1.40, 165.0), DBL_DIG);
	printf("1.40 (%19.*g) * 165. = %3.*g (rounded with %%3.%dg)\n", DBL_DIG, 1.40, DBL_DIG, mult_d(1.40, 165.0), DBL_DIG);
	printf("1.40 (%019.*g) * 165. = %03.*g (rounded with %%03.%dg)\n", DBL_DIG, 1.40, DBL_DIG, mult_d(1.40, 165.0), DBL_DIG);
	printf("1.40 (%#019.*g) * 165. = %#03.*g (rounded with %%#03.%dg)\n", DBL_DIG, 1.40, DBL_DIG, mult_d(1.40, 165.0), DBL_DIG);
	printf("1.40 (%19.*g) * 165. = %4.*g (rounded with %%4.%dg)\n", DBL_DIG, 1.40, DBL_DIG, mult_d(1.40, 165.0), DBL_DIG);
	printf("1.40 (%019.*g) * 165. = %04.*g (rounded with %%04.%dg)\n", DBL_DIG, 1.40, DBL_DIG, mult_d(1.40, 165.0), DBL_DIG);
	printf("1.40 (%#019.*g) * 165. = %#04.*g (rounded with %%#04.%dg)\n", DBL_DIG, 1.40, DBL_DIG, mult_d(1.40, 165.0), DBL_DIG);
	printf("1.40 (%#19.*g) * 165. = %#.*g (with %%#.%dg)\n", DBL_DIG, 1.40, DBL_DIG, mult_d(1.40, 165.0), DBL_DIG);
	printf("1.40 (%#19.*g) * 165. = %#1.*g (with %%#1.%dg)\n", DBL_DIG, 1.40, DBL_DIG, mult_d(1.40, 165.0), DBL_DIG);
	printf("1.40 (%19.*g) * 165. = %.*g (%%.%dg [DBL_DECIMAL_DIG])\n", my_DBL_DECIMAL_DIG, 1.40, my_DBL_DECIMAL_DIG, mult_d(1.40, 165.0), my_DBL_DECIMAL_DIG);
	printf("1.40 (%#19.*g) * 165. = %#.*g (%%#.%dg)\n", my_DBL_DECIMAL_DIG, 1.40, my_DBL_DECIMAL_DIG, mult_d(1.40, 165.0), my_DBL_DECIMAL_DIG);
	putchar('\n');

	puts("different widths of (3.0 / 7.0):");
	ldq = quot_ld(3.0L, 7.0L);	/* 0.428571 428571 428571 428571 ... */
	dq = quot_d(3.0, 7.0);
	fq = quot_f((float) 3.0f, (float) 7.0f);	/* xxx bucking "The Usual Conversions"! */

	if (dq == fq) {
		printf("float and double equate:  %#.*g\n", DBL_DIG, dq);
	} else {
		puts("WARNING: float and double are different:");
		printf(" dq = %#.*g\n", DBL_DIG, dq);
		printf(" fq = %#.*g\n", DBL_DIG, fq);

		if ((float) dq == fq) {
			printf("cast to float, double equates:  %#.*g\n", DBL_DIG, (float) dq);
		} else {
			puts("WARNING: even with cast to float, double is different:");
			printf(" dq = %#.*g\n", DBL_DIG, dq);
			printf(" fq = %#.*g\n", DBL_DIG, fq);
		}
	}

	if (ldq == (long double) dq) {
		printf("long double and double equate:  %#.*Lg\n", LDBL_DIG, ldq);
	} else {
		puts("WARNING: long double and double are different:");
		printf("ldq = %#.*Lg\n", LDBL_DIG, ldq);
		printf(" dq = %#.*g\n", DBL_DIG, dq);

		if ((double) ldq == dq) {
			printf("cast to double, long double equates: %#.*g\n", DBL_DIG, (double) ldq);
		} else {
			puts("WARNING: even with cast to double, long double is different:");
			printf("ldq = %#.*Lg\n", LDBL_DIG, ldq);
			printf(" dq = %#.*g\n", DBL_DIG, dq);
		}
	}
	putchar('\n');

	/* xxx GCC manual says the i386 ABI specifies a 96-bit long double */
	if (ldq == 3.0L / 7.0L) {
		puts("long double constant and runtime quotients are equal");
	} else {
		puts("WARNING: long double constant and runtime quotients are NOT equal (extended precision?)");
	}

	if (dq == 3.0 / 7.0) {
		puts("double constant and runtime quotients are equal");
	} else {
		puts("WARNING: double constant and runtime quotients are NOT equal (extended precision?)");
	}

	if (fq == 3.0f / 7.0f) {
		puts("float constant and runtime quotients are equal");
	} else {
		puts("WARNING: float constant and runtime quotients are NOT equal (extended precision?)");
	}

	putchar('\n');

	printf("interpreting IEEE 754 floating-point with %s byte order\n"
	       "                                      and %s bitfields.\n\n",
#if defined(__LITTLE_ENDIAN__)
	      "Little-Endian",
#else
	      "Big-Endian",
#endif
#ifdef _BIT_FIELDS_LTOH
	      "Low-to-High"
#else
	      "High-to-Low"
#endif
	     );

	printf("maximum integer in a float by ipow():\n");
	dq = ipow(FLT_RADIX, FLT_MANT_DIG); /* xxx may change value */
	print_drep(dq);

	printf("maximum integer in a float by shifting:\n");
	dq = 1LLU << FLT_MANT_DIG;
	print_drep(dq);

	printf("maximum integer in a double by ipow():\n");
	dq = ipow(FLT_RADIX, DBL_MANT_DIG); /* xxx may change value */
	print_drep(dq);

	printf("maximum integer in a double by shifting:\n");
	dq = 1LLU << DBL_MANT_DIG;
	print_drep(dq);

	printf("maximum integer in a double plus 1.0 (no change):\n");
	dq += 1.0;			/* N.B.:  does not change dq */
	print_drep(dq);

	printf("maximum integer in a double plus 2.0 (mantissa LS bit changes):\n");
	dq = 1LLU << DBL_MANT_DIG;
	dq += 2.0;			/* LS bit changes in mantissa */
	print_drep(dq);

	printf("double overflowed (infinity, > DBL_MAX):\n");
	dq = ipow(FLT_RADIX, DBL_MANT_DIG); /* xxx may change value */
	dq = dq * pow(10.0, 400.0);	/* inf */
	print_drep(dq);

	printf("double not quite overflowed (< DBL_MAX):\n");
	dq = ipow(FLT_RADIX, DBL_MANT_DIG);/* AKA maximum integer in a double *//* xxx may change value */
	dq = dq * pow(10.0, 308.0 - DBL_DIG); /* < DBL_MAX */
	print_drep(dq);

	printf("1 << 32, aka 2^32 [0x%llx]:\n", 1LLU << 32);
	dq = 1LLU << 32;
	print_drep(dq);

	printf("1 << 8 [0x%llx]:\n", 1LLU << 8);
	dq = 1LLU << 8;
	print_drep(dq);

	printf("just 0:\n");
	dq = 0;
	print_drep(dq);

	printf("memset(0x00):\n");
	memset(&dq, 0x00, sizeof(dq));
	print_drep(dq);

	printf("memset(0xFF):\n");
	memset(&dq, 0xff, sizeof(dq));
	print_drep(dq);			/* nan */

	printf("The smallest (de-normalised) number close to zero:\n");
	{
		my_double_t du;

		/*
		 * A stored exponent of zero means this is a de-normalised
		 * number.  The mantissa has no implicit leading bit but can
		 * have any number of bits set, and the mantissa is then scaled
		 * the same as if the stored exponent was 1.  So the smallest
		 * none-zero floating point number is indeed a stored exponent
		 * of 0, and a stored mantissa of 1.
		 *
		 * https://cs.stackexchange.com/a/127234
		 */
		du.i = 1;
		dq = du.d;
	}
	print_drep(dq);

	printf("give_me_one(4.2):\n");
	dq = give_me_one(4.2);
	print_drep(dq);

	printf("give_me_one(-10.5):\n");
	dq = give_me_one(-10.5);
	print_drep(dq);

	printf("give_me_one(553774558711019983333.9):\n");
	dq = give_me_one(553774558711019983333.9);
	print_drep(dq);

	printf("just 1:\n");
	dq = 1;
	print_drep(dq);

	printf("just -1:\n");
	dq = -1;
	print_drep(dq);

	printf("DBL_MAX(%s):\n", ___STRING(DBL_MAX));
	dq = DBL_MAX;			/* 1.7976931348623157E+308 */
	print_drep(dq);			/* 1.797693134862316E+308 */

	printf("DBL_MIN(%s):\n", ___STRING(DBL_MIN));
	dq = DBL_MIN;			/* 2.2250738585072014E-308 */
	print_drep(dq);			/* 2.225073858507201E-308 */

	printf("-DBL_MAX(-%s):\n", ___STRING(DBL_MAX));
	dq = -DBL_MAX;			/* -1.7976931348623157E+308 */
	print_drep(dq);			/* -1.797693134862316E+308 */

	printf("-DBL_MIN(-%s):\n", ___STRING(DBL_MIN));
	dq = -DBL_MIN;			/* -2.22507385850720E-308 */
	print_drep(dq);			/* -2.22507385850720E-308 */

	printf("DBL_MIN + 3.278:\n");
	dq += 3.278;
	print_drep(dq);

#if defined(HAVE_FPGETROUND)
	printf("fpgetround() = %s\n", fpround2nm(fpgetround()));
#endif
#if (! defined(HAVE_IEEEFP_H) &&                                        \
     (defined(__STDC_VERSION__) &&                                      \
      ((__STDC_VERSION__ - 0) >= 199901L)) ||                           \
     defined(HAVE_FEGETROUND))
	printf("fegetround() = %s\n", feround2nm(fegetround()));
#endif
	dq = 1.1111111111114850E+100;
	print_drep(dq);			/* all round up, as expected */

	dq = 1.1111111111114950E+100;
	printf("%%.50g = %.50g", dq);
	print_drep(dq);			/* none round up, as is represented as just less */

	dq = 1.1111111111114951E+100;
	print_drep(dq);			/* print_decimal() fails to round ifndef PRINT_ROUND_TRIP_SAFE */

	dq = 1.1111111111114960E+100;
	print_drep(dq);			/* print_decimal() fails to round ifndef PRINT_ROUND_TRIP_SAFE */

	dq = 1.1111111111114955E+100;
	print_drep(dq);			/* print_decimal() fails to round ifndef PRINT_ROUND_TRIP_SAFE */

	printf("just 0.1:\n");
	dq = 0.1;
	print_drep(dq);

	printf("just -0.1:\n");
	dq = -0.1;
	print_drep(dq);

	/*
	 * 16:34 [563] $ emacs --batch --eval '(print (+ 0.1 0.11))'
	 * 
	 * 0.21000000000000002
	 * 16:34 [563] $ emacs --batch --eval '(let ((float-output-format "%.15g")) (print (+ 0.1 0.11)))'
	 * 
	 * 0.21
	 *
	 * 10:29 [539] $ python
	 * Python 2.6.4 (r264:75706, Feb 17 2015, 15:51:53) 
	 * [GCC 4.1.3 20080704 prerelease (NetBSD nb3 20111107)] on netbsd5
	 * Type "help", "copyright", "credits" or "license" for more information.
	 * >>> 0.1 + 0.11
	 * 0.21000000000000002
	 * >>> 
	 */
	printf("0.1 + 0.11:\n");
	dq = 0.1;
	dq += 0.11;
	print_drep(dq);

	printf("just 0.21 (compare to above!):\n");
	dq = 0.21;
	print_drep(dq);

	printf("just 1.4:\n");
	dq = 1.4;
	print_drep(dq);

	printf("1.4 * 165:\n");
	dq = 1.4 * 165;
	print_drep(dq);

	printf("1.0 / 7.0:\n");
	dq = 1.0 / 7.0;			/* 0.142857 142857 14287 14287 ...  */
	print_drep(dq);

	printf("1 << 32 + 48692:\n");
	dq = 1LLU << 32;
	dq += 48592;
	print_drep(dq);

	printf("1 << 32 + 48692 + 3.278:\n");
	dq += 3.278;
	print_drep(dq);

	dq = 0;
	printf("0 + 3.278:\n");
	dq += 3.278;
	print_drep(dq);

	printf("0 + 3.278 + 48592:\n");
	dq += 48592;
	print_drep(dq);

	printf("just -3.278:\n");
	dq = -3.278;
	print_drep(dq);

	printf("just -0.42:\n");
	dq = -0.42;
	print_drep(dq);

	printf("just -0.042:\n");
	dq = -0.042;
	print_drep(dq);

	printf("just -0.0042:\n");
	dq = -0.0042;
	print_drep(dq);

	printf("just -0.00042:\n");
	dq = -0.00042;
	print_drep(dq);

	printf("just -0.000042:\n");
	dq = -0.000042;
	print_drep(dq);

	printf("just -0.0000042:\n");
	dq = -0.0000042;
	print_drep(dq);

	printf("just -0.00000042:\n");
	dq = -0.00000042;
	print_drep(dq);

	printf("just -0.00000000000000000042:\n");
	dq = -0.00000000000000000042;
#ifndef PRINT_ROUND_TRIP_SAFE
	/* xxx does not round correctly: -4.1999999999999998e-19 */
#endif
	print_drep(dq);

	printf("just -0.000042:\n");
	dq = 0.000042;
	print_drep(dq);

	printf("the mass of a proton in kilograms:\n");
	dq = 1.67262158e-27;
	print_drep(dq);

	dq = 1.0 - 0.9 - 0.1;	/* does not round correctly at max precision! */
	printf("XXX: this should round to zero, but does not!\n");
	printf("log10(1.0 - 0.9 - 0.1) = %1.*e:\n", DBL_DIG - 1, log10(dq));
	print_drep(dq);		/* possibly the log10() result is wrong?????? */

	printf("0.99 * 0.99:\n");
	dq = 0.99 * 0.99;
#ifndef PRINT_ROUND_TRIP_SAFE
	/* xxx does not round correctly: 9.8009999999999997e-01 */
#endif
	print_drep(dq);

	printf("56789.012345 / 1111111111.0 / 1000000000000.0:\n");
	dq = 56789.012345 / 1111111111.0;
	dq /= 1000000000000.0;
	print_drep(dq);

	printf("1.4 * 165 * 10.0^20.0:\n");
	dq = 1.4 * 165;
	dq *= pow(10.0, 20.0);
	print_drep(dq);

	printf("0.70 + 0.20 + 0.10:\n");
	dq = 0.70 + 0.20 + 0.10;
	print_drep(dq);

	printf("1.0 - 0.9 - 0.1:\n");
	dq = 1.0 - 0.9 - 0.1;
	print_drep(dq);

	printf("10.1 - 9.93:\n");
	dq = 10.1 - 9.93;
	print_drep(dq);

	printf("just 999999999999999 (15 9s):\n");
	dq = 999999999999999LLU;
	print_drep(dq);

	printf("just 999999999999999 (15 9s):\n");
	dq = 999999999999999LLU;
	print_drep(dq);

	printf("just 10000000000000000 (15 0s):\n");
	dq = 1000000000000000LLU;
	print_drep(dq);

	printf("XXX: rounds up!  just 9999999999999999 (16 9s):\n");
	dq = 9999999999999999LLU;	/* warning: conversion from 'long int' to 'double' changes value from '9999999999999999' to '1.0e+16' [-Wfloat-conversion] */
	print_drep(dq);

	printf("just 10000000000000000 (16 0s):\n");
	dq = 10000000000000000LLU;
	print_drep(dq);

	printf("XXX: rounds up!  just 99999999999999999 (17 9s):\n");
	dq = 99999999999999999LLU;	/* warning: conversion from 'long int' to 'double' changes value from '99999999999999999' to '1.0e+17' [-Wfloat-conversion] */
	print_drep(dq);

	printf("just 100000000000000000 (17 0s):\n");
	dq = 100000000000000000LLU;
	print_drep(dq);

	printf("XXX: rounds up!  just 999999999999999999 (18 9s):\n");
	dq = 999999999999999999LLU;	/* warning: conversion from 'long int' to 'double' changes value from '999999999999999999' to '1.0e+18' [-Wfloat-conversion] */
	print_drep(dq);

	printf("just 1000000000000000000 (18 0s):\n");
	dq = 1000000000000000000LLU;
	print_drep(dq);

	printf("XXX: rounds up!  just 9999999999999999999U (19 9s):\n");
	dq = 9999999999999999999LLU;	/* warning: conversion from 'long unsigned int' to 'double' changes value from '9999999999999999999' to '1.0e+19' [-Wfloat-conversion] */
	print_drep(dq);

	printf("just 10000000000000000000 (19 0s):\n");
	dq = 10000000000000000000LLU;
	print_drep(dq);

	printf("XXX: rounds down!  just -9999999999999999 (16 9s):\n");
	dq = -9999999999999999LL;	/* warning: conversion from 'long int' to 'double' changes value from '9999999999999999' to '1.0e+16' [-Wfloat-conversion] */
	print_drep(dq);

	printf("just -10000000000000000 (16 0s):\n");
	dq = -10000000000000000LL;
	print_drep(dq);

	printf("XXX: rounds down!  just -99999999999999999 (17 9s):\n");
	dq = -99999999999999999LL;	/* warning: conversion from 'long int' to 'double' changes value from '99999999999999999' to '1.0e+17' [-Wfloat-conversion] */
	print_drep(dq);

	printf("just -100000000000000000 (17 0s):\n");
	dq = -100000000000000000LL;
	print_drep(dq);

	printf("XXX: rounds down!  just -999999999999999999 (18 9s):\n");
	dq = -999999999999999999LL;	/* warning: conversion from 'long int' to 'double' changes value from '999999999999999999' to '1.0e+18' [-Wfloat-conversion] */
	print_drep(dq);

	printf("just -1000000000000000000 (18 0s):\n");
	dq = -1000000000000000000LL;
	print_drep(dq);

	printf("XXX: rounds down!  just -9999999999999999999U (19 9s):\n");
	dq = -9999999999999999999LLU;	/* warning: conversion from 'long unsigned int' to 'double' changes value from '9999999999999999999' to '1.0e+19' [-Wfloat-conversion] */
	print_drep(dq);

	printf("just -10000000000000000000 (19 0s):\n");
	dq = -10000000000000000000LL;	/* warning: integer constant is so large that it is unsigned */
	print_drep(dq);

	return;
}



static void
precision()				/* xxx clang is getting mouthy */
{
	unsigned int ldbl_log10;
	unsigned int ui;
	intmax_t ti;
	float tf;
	double td;
	long double tld;


	/*
	 * We must round the decimal presentation of a binary floating point
	 * number to the appropriate, limited, number of digits to avoid
	 * presenting invalid junk digits.  So, how many digits can we print and
	 * still get as much precision as possible?  {FLT,DBL,LDBL}_DIG of course!
	 *
	 * Calculating the {FLT,DBL,LDBL}_DIG values at runtime:
	 *
	 * These give the maximum number of decimal (base 10) digits in any
	 * floating point number which can be stored without any loss of
	 * precision (i.e. without change to any digit).  More accurately:
	 *
	 *	The number of decimal digits, 'q', such that any floating-point
	 *	number with 'q' decimal digits can be rounded into a
	 *	floating-point number with 'p of radix r' digits and back again
	 *	without change to the 'q' decimal digits.
	 *
	 * I.e. the number of decimal digits that survive a round-trip of
	 * decimal -> binary -> decimal for a given number of binary digits.
	 *
	 * Note to start with that the largest magnitude number that can be
	 * represented by a list of 'p of radix r' digits is literally 'r to
	 * the power of p' in a given representation (e.g. log10() gives the
	 * order of magnitude for a base-10 representation), so to find the
	 * largest magnitude base-10 number that can be represented we would
	 * use (rounding down to account for full magnitudes):
	 *
	 *	floor(log10(pow(r, p)))
	 *
	 * We can optimize the math to avoid the 'x ** y' operation and deal
	 * with the special case of binary-coded decimal:
	 *
	 *	p * log10(r)                    if r is a power of 10
	 * or
	 *	floor(p * log10(r))             otherwise
	 *
	 * However for the binary internal represenation used in IEEE 754 we
	 * must subtract one bit from the mantissa (p):
	 *
	 *	p * log10(r)                    if r is a power of 10
	 *
	 *	floor((p - 1) * log10(r))       otherwise
	 *
	 * The reason for subtracting one bit (thus giving the value of 6 and
	 * not 7 for FLT_DIG, for example), is due to rounding errors -- not
	 * all FLOATING POINT values with 7 decimal digits can be losslessly
	 * represented by a 32-bit float.  Rounding errors are limited to 1 bit
	 * though, so calculating FLT_DIG based on 23 bits (instead of the full
	 * 24) is safe.
	 *
	 * Put another way, the standard 32-bit binary IEEE 754 floating-point
	 * type has a 24 bit fractional part (mantissa) (23 bits stored, one
	 * implied), which at first may suggest that it can represent 7 digit
	 * decimal digits:
	 *
	 *	floor(log10(2^24)) = 7
	 *	floor(24 * log10(2)) = 7
	 *
	 * However relative rounding errors are non-uniform and some floating
	 * point values with 7 decimal digits do not survive conversion to
	 * 32-bit float and back.  The smallest positive example is 8.589973e9,
	 * which becomes 8.589974e9 after the round-trip.  These rounding errors
	 * cannot exceed one bit in the binary representation, and so FLT_DIG is
	 * calculated as
	 *
	 *	floor((24 - 1) * log10(2)) = 6
	 *
	 * xxx the comment for FLT_DIG in float_ieee754.h says:
	 *
	 *	floor((mant - 1) * log10(radix)) + (radix == 10)
	 *
	 * Of course it really should be generalized to avoid using the value
	 * of 'true' in a logical expression:
	 *
	 *	floor((mant - 1) * log10(radix)) + (((radix % 10) == 0) ? 1 : 0)
	 *
	 * So for binary (base-2) floating point it is just:
	 *
	 *	floor((mant - 1) * log10(2))
	 *
	 * or for binary (base-2) floating point using plain log():
	 *
	 *	floor((mant - 1) * (log(2) / log(10)))
	 *
	 * Note:  we can optimize even further if (FLT_RADIX == 2) && the
	 * architecture is 2's-complement:
	 *
	 * 	ilog10(1LLU << (mant - 1))
	 *
	 *
	 * The inverse, calculating decimal digits needed for 'b' bits is:
	 *
	 *	ceil(mant * (log(2) / log(10))) + 1
	 *
	 * Note also that the *_MANT_DIG values are already one greater than
	 * the actual number of bits stored in the mantissa since if a mantissa
	 * contains 'abcdef...' then the number it represents is actually
	 * '1.abcdef... × 2^e', thus it provides one extra implicit bit of
	 * precision.
	 *
	 * Note:  As shown above in report() the number of significant digits
	 * after the decimal place when a floating point number with a
	 * magnitude between 1.0 and 1.0eDBL_DIG formatted in base-10, e.g. for
	 * "%.*f", is:
	 *
	 *	DBL_DECIMAL_DIG - (int) lrint(floor(log10(ceil(fabs(1.0))))) + 1
	 */

	/*
	 * so, we use this as the format width:  ceil(mant * log10(2))
	 *
	 * XXX hmmmm....:  ilog10(uipow(FLT_RADIX, LDBL_MANT_DIG - 1))
	 */
	ldbl_log10 = (unsigned int) calc_LDBL_DECIMAL_DIG;
#if 0
	printf("Note:  rounding to %d digits for 'long double' display...\n\n", ldbl_log10);
#endif
	/*
	 * Calculate the absolute maximum magnitude integer that can be safely
	 * stored in a floating point variable without losing precision.
	 *
	 * (MATLAB calls these "flintmax", or the "largest consecutive Integer
	 * in floating point format")
	 *
	 * N.B.:  to get the format width we must add one to the number of
	 * base-10 digits which accounts for for the most significant digit,
	 * which cannot be counted in the rounding magnitudes
	 */
	printf("Max magnitude base-10 integer in a float:\n"
	       "       = %*ju\n",
	       ldbl_log10, uipow(FLT_RADIX, FLT_MANT_DIG));
	printf("    calculated using base-2 shifts:\n"
	       "       = %*ju\n",
	       ldbl_log10, (uintmax_t) (1LLU << FLT_MANT_DIG));
	tf = ti = (1LLU << FLT_MANT_DIG); /* xxx may change value */
	tf += 1.0f;
	ti += 1;
	if ((long long int) tf != (long long int) ti) {
		printf("NOTICE:               %.4f + 1.0 !=          %ju + 1 !!!\n",
		       (float) (1LLU << FLT_MANT_DIG),
		       (uintmax_t) (1LLU << FLT_MANT_DIG));
	}
	putchar('\n');
	tf = ti = (1LLU << FLT_MANT_DIG); /* xxx may change value */
	ti -= 2;
	tf -= 2.0f;
	for (ui = 0; ui <= 4; ui++, ti++) {
		/* the addition must be done all at once, not incrementally */
		tf = (1LLU << FLT_MANT_DIG);
		tf -= (float) 2.0;
		tf += ui * 1.0;		/* xxx may change value */
		printf("ti + %u = %*jd,  tf + %u = %*.10f\n",
		       ui, ldbl_log10, ti,
		       ui, ldbl_log10 + 10, tf);
	}
	putchar('\n');

	/*  */ {
		
	uintmax_t tui;

	printf("Max magnitude base-10 integer in a double:\n"
	       "       = %*ju\n",
	       ldbl_log10, uipow(FLT_RADIX, DBL_MANT_DIG));
	printf("    calculated using base-2 shifts:\n"
	       "       = %*ju\n",
	       ldbl_log10, (uintmax_t) (1LLU << DBL_MANT_DIG));
	td = tui = (1LLU << DBL_MANT_DIG); /* xxx may change value */
	td += 1.0;
	tui += 1;
	if ((uintmax_t) td != tui) {
		printf("NOTICE:       %.4f + 1.0 !=  %ju + 1 !!!\n",
		       (double) (1LLU << DBL_MANT_DIG),
		       (uintmax_t) (1LLU << DBL_MANT_DIG));
	}
	putchar('\n');
	td = tui = (1LLU << DBL_MANT_DIG); /* xxx may change value */
	tui -= 2;
	td -= 2.0;
	for (ui = 0; ui <= 4; ui++, tui++) {
		/* the addition must be done all at once, not incrementally */
		td = (1LLU << DBL_MANT_DIG);
		td -= 2.0;
		td += ui * 1.0;
		printf("tui+ %u = %*ju,  td + %u = %*.10f\n",
		       ui, ldbl_log10, tui,
		       ui, ldbl_log10 + 10, td);
#if 0
		printf("tui+ %u = %*ju,  td + %u =   %1.*e\n",
		       ui, ldbl_log10, tui,
		       ui, calc_LDBL_DECIMAL_DIG, td);
#endif
	}
	putchar('\n');
		
	}

#ifdef __SIZEOF_INT128__
	{
		
	__uint128_t tlui;

	/* N.B.:  because:  (FLT_RADIX ** LDBL_MANT_DIG) > sizeof(uintmax_t) * CHAR_BIT */
	tld = powl((long double) FLT_RADIX, (long double) LDBL_MANT_DIG);
	printf("Max magnitude base-10 integer in a long double:\n"
	       "        = %*.0Lf\n",
	       ldbl_log10, tld);
	tlui = (__uint128_t) tld;
	printf("    converted to __int128_t:\n"
	       "        = ");
	print_u128(tlui);
	printf("%s\n", tlui == (__uint128_t) tld ? "" : " (not equal)");
	printf("    calculated using base-2 shifts\n"
	       "        = ");
	print_u128((__uint128_t) 1 << LDBL_MANT_DIG);
	putchar('\n');
	tld = tlui = ((__uint128_t) 1 << LDBL_MANT_DIG); /* xxx may change value */
	tld += 1.0L;
	tlui += 1;
	if ((__uint128_t) tld != tlui) {
		printf("NOTICE:   %.1Lf + 1.0 != ",
		       (long double) ((__uint128_t) 1 << LDBL_MANT_DIG));
		print_u128((__uint128_t) 1 << LDBL_MANT_DIG);
		printf(" + 1 !!!\n");
	}
	putchar('\n');
	tld = tlui = ((__uint128_t) 1 << LDBL_MANT_DIG); /* xxx may change value */
	/*
	 * XXX Hmmmm....  GCC-4.1.3 20080704 on NetBSD-5 is broken --
	 * the subtraction here fails to do anything
	 *
	 * It is OK with GCC 4.8.4 20150115 on NetBSD-7.99.5
	 *
	 * It is OK with GCC 4.2.1 on OSX 10.6.8
	 *
	 * It is OK with Clang 1.7 (LLVM 2.9svm) on OSX 10.6.8 [and newer]
	 */
	tld -= 4.0;
	tlui -= 4;
	for (ui = 0; ui <= 8; ui++, tlui++) {
		/* the addition must be done all at once, not incrementally */
		tld = ((__uint128_t) 1 << LDBL_MANT_DIG);
		tld -= 4.0L;
		tld += ui * 1.0L;
		printf("tlui+ %u = ", ui);
		print_u128(tlui);
		printf(", tld + %u = %*.10Lf\n",
		       ui, ldbl_log10 + 10, tld);
#if 0
		printf("tlui+ %u = ", ui);
		print_u128(tlui);
		printf(", tld + %u =%1.*Le\n",
		       ui, LDBL_DIG - 1, tld);
#endif
	}
	putchar('\n');
		
	}
#endif	/* __SIZEOF_INT128__ */

	printf("FLT_RADIX     = %2d\n", FLT_RADIX);

	printf("FLT_MANT_DIG  = %d\n", FLT_MANT_DIG);
	printf("DBL_MANT_DIG  = %d\n", DBL_MANT_DIG);
	printf("LDBL_MANT_DIG = %d\n", LDBL_MANT_DIG);

	putchar('\n');

	printf("FLT_prec:    %g\n", floor((FLT_MANT_DIG - 1) * log10(2)));
	printf("FLT_ilog10:  %d\n", ilog10(1LLU << (FLT_MANT_DIG - 1)));
	printf("FLT_sigdig:  %d\n", ilog10(uipow(FLT_RADIX, FLT_MANT_DIG - 1)));
	printf("FLT_hdtoa:   %d\n", (FLT_MANT_DIG + 3) / 4); /* xxx from src/lib/ibc/gdtoa/hdtoa.c */
	putchar('\n');
	printf("DBL_prec:    %g\n", floor((DBL_MANT_DIG - 1) * log10(2)));
	printf("DBL_ilog10:  %d\n", ilog10(1LLU << (DBL_MANT_DIG - 1)));
	printf("DBL_sigdig:  %d\n", ilog10(uipow(FLT_RADIX, DBL_MANT_DIG - 1)));
	printf("DBL_hdtoa:   %d  XXX wrong?\n", (DBL_MANT_DIG + 3) / 4); /* xxx from src/lib/ibc/gdtoa/hdtoa.c */
	putchar('\n');
	printf("LDBL_prec:   %g\n", floor((LDBL_MANT_DIG - 1) * log10(2)));
	printf("LDBL_ilog10: %d\n", ilog10(1LLU << (LDBL_MANT_DIG - 1)));
	printf("LDBL_sigdig: %d\n", ilog10(uipow(FLT_RADIX, LDBL_MANT_DIG - 1)));
	printf("LDBL_hldtoa: %d  XXX very wrong\n", (LDBL_MANT_DIG + 3) / 4); /* xxx from src/lib/ibc/gdtoa/hdtoa.c */

	putchar('\n');

	printf("ceil_ilog10(UINT64_MAX) = %d\n\n", ceil_ilog10(UINT64_MAX));

	putchar('\n');

	printf("FLT_DIG              = %2d\n", FLT_DIG);
	if (ilog10(uipow(FLT_RADIX, FLT_MANT_DIG - 1)) != FLT_DIG) {
		printf("WARNING:  should be  = %2u\n", ilog10(uipow(FLT_RADIX, FLT_MANT_DIG - 1)));
	}
	if (ilog10((1LLU << (FLT_MANT_DIG - 1)) - 1) != FLT_DIG) {
		printf("NOTICE: powers of 2  = %2u\n", ilog10((1LLU << (FLT_MANT_DIG - 1)) - 1));
	}
	if (ilog10(uipow(FLT_RADIX, FLT_MANT_DIG - 1)) !=
	    llrint(floor(log10(pow((double) FLT_RADIX, (double) FLT_MANT_DIG - 1))))) {
		printf("WARNING:  Hmmmm....     %u by int != %lld by double\n",
		       ilog10(uipow(FLT_RADIX, FLT_MANT_DIG - 1)),
		       llrint(floor(log10(pow((double) FLT_RADIX, (double) FLT_MANT_DIG - 1)))));
	}
	if (llrint(floor((double) (FLT_MANT_DIG - 1) * log10((double) FLT_RADIX))) != FLT_DIG) {
		printf("WARNING:  Hmmmm....     %lld by alternate double\n",
		       llrint(floor((double) (FLT_MANT_DIG - 1) * log10((double) FLT_RADIX))));
	}

	printf("DBL_DIG              = %2d\n", DBL_DIG);
	if (ilog10(uipow(FLT_RADIX, DBL_MANT_DIG - 1)) != DBL_DIG) {
		printf("WARNING:  should be  = %u\n", ilog10(uipow(FLT_RADIX, DBL_MANT_DIG - 1)));
	}
	if (ilog10((1LLU << (DBL_MANT_DIG - 1)) - 1) != DBL_DIG) {
		printf("NOTICE: powers of 2  = %u\n", ilog10((1LLU << (DBL_MANT_DIG - 1)) - 1));
	}
	if (ilog10(uipow(FLT_RADIX, DBL_MANT_DIG - 1)) !=
	    llrint(floor(log10(pow((double) FLT_RADIX, (double) DBL_MANT_DIG - 1))))) {
		printf("WARNING:  Hmmmm....    %u by int != %lld by double\n",
		       ilog10(uipow(FLT_RADIX, DBL_MANT_DIG - 1)),
		       llrint(floor(log10(pow((double) FLT_RADIX, (double) DBL_MANT_DIG - 1)))));
	}
	if (llrint(floor((double) (DBL_MANT_DIG - 1) * log10((double) FLT_RADIX))) != DBL_DIG) {
		printf("WARNING:  Hmmmm....    %lld by alternate double\n",
		       llrint(floor((double) (DBL_MANT_DIG - 1) * log10((double) FLT_RADIX))));
	}

	printf("LDBL_DIG             = %2d\n", LDBL_DIG);
	if (llrint(floor(log10(pow((double) FLT_RADIX, (double) LDBL_MANT_DIG - 1)))) != LDBL_DIG) {
		printf("WARNING:  should be  = %lld\n",
		       llrint(floor(log10(pow((double) FLT_RADIX, (double) LDBL_MANT_DIG - 1)))));
	}
#ifdef __SIZEOF_LONG_LONG__
/*
 * xxx assume intmax_t is long long, _and_ 128 bits!
 */
# if __SIZEOF_LONG_LONG__ > __SIZEOF_LONG__
	if (ilog10((1LLU << (LDBL_MANT_DIG - 1)) - 1) != LDBL_DIG) {
		printf("NOTICE: powers of 2  = %u\n", ilog10(((uintmax_t) 1 << (LDBL_MANT_DIG - 1)) - 1));
	}
# endif
#endif
	if (llrint(floor((double) (LDBL_MANT_DIG - 1) * log10((double) FLT_RADIX))) != LDBL_DIG) {
		printf("WARNING:  Hmmmm....    %lld by alternate double\n",
		       llrint(floor((double) (LDBL_MANT_DIG - 1) * log10((double) FLT_RADIX))));
	}
	putchar('\n');

	/*
	 * *_DECIMAL_DIG:  Minimum decimal digits for binary->decimal->binary
	 * round-trip conversion.
	 *
	 * I.e. the minimum number of decimal digits required for a binary
	 * value of 'b' bits to round-trip, i.e. the inverse of the
	 * {FLT,DBL,LDBL}_DIG values, AKA:
	 *
	 * xxx the comment for FLT_DIG in float_ieee754.h says:
	 *
	 *	ceil(1 + (mant * log10(radix))) - (radix == 10)
	 *
	 * Of course it really should be generalized to avoid using the value
	 * of 'true' (and take the constant "+1" out of the inner expression):
	 *
	 *	ceil(mant * log10(radix)) + 1 - (((radix % 10) == 0) ? 1 : 0)
	 *
	 * or for binary (base-2) floating point:
	 *
	 *	ceil(mant * log10(2)) + 1
	 *
	 * or for binary (base-2) floating point using plain log():
	 *
	 *	ceil(mant * (log(2) / log(10))) + 1
	 */
#ifdef FLT_DECIMAL_DIG
	printf("FLT_DECIMAL_DIG  = %d\n", FLT_DECIMAL_DIG);
	if (calc_FLT_DECIMAL_DIG != FLT_DECIMAL_DIG) {
		printf("WARNING:  should be  = %d\n", calc_FLT_DECIMAL_DIG);
	}
#else
	printf("NOTE:  FLT_DECIMAL_DIG should be  = %d\n", calc_FLT_DECIMAL_DIG);
#endif

#ifdef DBL_DECIMAL_DIG
	printf("DBL_DECIMAL_DIG  = %d\n", DBL_DECIMAL_DIG);
	if (calc_DBL_DECIMAL_DIG != DBL_DECIMAL_DIG) {
		printf("WARNING:  should be  = %d\n", calc_DBL_DECIMAL_DIG);
	}
#else
	printf("NOTE:  DBL_DECIMAL_DIG should be  = %d\n", calc_DBL_DECIMAL_DIG);
#endif

#ifdef LDBL_DECIMAL_DIG
	printf("LDBL_DECIMAL_DIG = %d\n", LDBL_DECIMAL_DIG);
	if (calc_LDBL_DECIMAL_DIG != LDBL_DECIMAL_DIG) {
		printf("WARNING:  should be  = %d\n", calc_LDBL_DECIMAL_DIG);
	}
#else
	printf("NOTE:  LDBL_DECIMAL_DIG should be = %d\n", calc_LDBL_DECIMAL_DIG);
#endif

/* xxx assuming "long double" is supported and is the widest float... */
#ifdef DECIMAL_DIG		/* xxx this may have come from __DECIMAL_DIG__ */
	printf("DECIMAL_DIG          = %d\n", DECIMAL_DIG);
	if (calc_LDBL_DECIMAL_DIG != DECIMAL_DIG) {
		printf("WARNING:  should be  = %d\n", calc_LDBL_DECIMAL_DIG);
	}
#else
	printf("NOTE:  DECIMAL_DIG should be  = %2d\n", calc_LDBL_DECIMAL_DIG);
#endif
	putchar('\n');

#ifdef SNG_EXP_BIAS
	printf("SNG_EXP_BIAS =  %d\n", SNG_EXP_BIAS);
	if (SNG_EXP_BIAS != calc_SNG_EXP_BIAS) {
		printf("WARNING:  should be  =  %d\n", calc_SNG_EXP_BIAS);
	}
#else
	printf("NOTE:  SNG_EXP_BIAS should be =  %d\n", my_SNG_EXP_BIAS);
#endif
#ifdef DBL_EXP_BIAS
	printf("DBL_EXP_BIAS =  %d\n", DBL_EXP_BIAS);
	if (DBL_EXP_BIAS != calc_DBL_EXP_BIAS) {
		printf("WARNING:  should be  =  %d\n", calc_DBL_EXP_BIAS);
	}
#else
	printf("NOTE:  DBL_EXP_BIAS should be =  %d\n", my_DBL_EXP_BIAS);
#endif
#ifdef LDBL_EXP_BIAS
	printf("LDBL_EXP_BIAS = %d\n", LDBL_EXP_BIAS);
	if (LDBL_EXP_BIAS != calc_LDBL_EXP_BIAS) {
		printf("WARNING:  should be  = %d\n", calc_LDBL_EXP_BIAS);
	}
#else
	printf("NOTE:  LDBL_EXP_BIAS should be = %d\n", my_LDBL_EXP_BIAS);
#endif

	putchar('\n');

#ifdef SNG_EXPBITS
	printf("SNG_EXPBITS  =  %d\n", SNG_EXPBITS);
	if (SNG_EXPBITS != calc_SNG_EXPBITS) {
		printf("WARNING:  should be  =  %d\n", calc_SNG_EXPBITS);
	}
#else
	printf("NOTE:  SNG_EXPBITS should be =  %d\n", my_SNG_EXPBITS);
#endif
#ifdef SNG_FRACBITS
	printf("SNG_FRACBITS =  %d\n", SNG_FRACBITS);
	if (SNG_FRACBITS != calc_SNG_FRACBITS) {
		printf("WARNING:  should be  =  %d\n", calc_SNG_FRACBITS);
	}
#else
	printf("NOTE:  SNG_FRACBITS should be = %d\n", my_SNG_FRACBITS);
#endif

	putchar('\n');

#ifdef DBL_EXPBITS
	printf("DBL_EXPBITS  =  %d\n", DBL_EXPBITS);
	if (DBL_EXPBITS != calc_DBL_EXPBITS) {
		printf("WARNING:  should be  =  %d\n", calc_DBL_EXPBITS);
	}
#else
	printf("NOTE:  DBL_EXPBITS should be =  %d\n", my_DBL_EXPBITS);
#endif
#ifdef DBL_FRACBITS
	printf("DBL_FRACBITS =  %d\n", DBL_FRACBITS);
	if (DBL_FRACBITS != calc_DBL_FRACBITS) {
		printf("WARNING:  should be  =  %d\n", calc_DBL_FRACBITS);
	}
#else
	printf("NOTE:  DBL_FRACBITS should be = %d\n", my_DBL_FRACBITS);
#endif

	putchar('\n');

	printf("FLT_MIN      = %a\n", FLT_MIN);
	printf("FLT_MIN      = %1.*e (rounded)\n", FLT_DIG, FLT_MIN);
	printf("FLT_MIN      = %1.*e\n", my_FLT_DECIMAL_DIG, FLT_MIN);
	printf("FLT_MIN      ='%s'\n", ___STRING(FLT_MIN));

	printf("FLT_MAX      = %a\n", FLT_MAX);
	printf("FLT_MAX      = %1.*e (rounded)\n", FLT_DIG, FLT_MIN);
	printf("FLT_MAX      = %1.*e\n", my_FLT_DECIMAL_DIG, FLT_MAX);
	printf("FLT_MAX      ='%s'\n", ___STRING(FLT_MAX));

	putchar('\n');

	printf("FLT_EPSILON  = %a\n", FLT_EPSILON);
	printf("FLT_EPSILON  = %1.*e (rounded)\n", FLT_DIG, FLT_EPSILON);
	printf("FLT_EPSILON  = %1.*e\n", my_FLT_DECIMAL_DIG, FLT_EPSILON);
	printf("FLT_EPSILON  ='%s'\n", ___STRING(FLT_EPSILON));
	/* xxx should check these.... */
	printf("FLT_MIN_EXP     = %d\n", FLT_MIN_EXP);
	printf("FLT_MAX_EXP     = %d\n", FLT_MAX_EXP);
	printf("FLT_MIN_10_EXP  = %d\n", FLT_MIN_10_EXP);
	printf("FLT_MIN_10_EXP  = %d [calculated]\n", (int) lrint(floor(log10(fabs(FLT_MIN)))) + 1);	/* xxx explain the +1 */
	printf("FLT_MAX_10_EXP  = %d\n", FLT_MAX_10_EXP);
	printf("FLT_MAX_10_EXP  = %d [calculated]\n", (int) lrint(floor(log10(fabs(FLT_MAX)))));

	putchar('\n');
	putchar('\n');

	printf("DBL_MIN      = %a\n", DBL_MIN);
	printf("DBL_MIN      = %1.*e (rounded)\n", DBL_DIG, DBL_MIN);
	printf("DBL_MIN      = %1.*e\n", my_DBL_DECIMAL_DIG, DBL_MIN);
	printf("DBL_MIN      ='%s'\n", ___STRING(DBL_MIN));

	putchar('\n');

	printf("DBL_MAX      = %a\n", DBL_MAX);
	printf("DBL_MAX      = %1.*e (rounded)\n", DBL_DIG, DBL_MAX);
	printf("DBL_MAX      = %1.*e\n", my_DBL_DECIMAL_DIG, DBL_MAX);
	printf("DBL_MAX      ='%s'\n", ___STRING(DBL_MAX));

	putchar('\n');

	printf("DBL_EPSILON  = %a\n", DBL_EPSILON);
	printf("DBL_EPSILON  = %1.*e (rounded)\n", DBL_DIG, DBL_EPSILON);
	printf("DBL_EPSILON  = %1.*e\n", my_DBL_DECIMAL_DIG, DBL_EPSILON);
	printf("DBL_EPSILON  ='%s'\n", ___STRING(DBL_EPSILON));
	/* xxx should check these.... */
	printf("DBL_MIN_EXP     = %d\n", DBL_MIN_EXP);
	printf("DBL_MAX_EXP     = %d\n", DBL_MAX_EXP);
	printf("DBL_MIN_10_EXP  = %d\n", DBL_MIN_10_EXP);
	printf("DBL_MIN_10_EXP  = %d [calculated]\n", (int) lrint(floor(log10(fabs(DBL_MIN)))) + 1);	/* xxx explain the +1 */
	printf("DBL_MAX_10_EXP  = %d\n", DBL_MAX_10_EXP);
	printf("DBL_MAX_10_EXP  = %d [calculated]\n", (int) lrint(floor(log10(fabs(DBL_MAX)))));

	putchar('\n');
	putchar('\n');

	printf("LDBL_MIN      = %La\n", LDBL_MIN);
	printf("LDBL_MIN      = %1.*Le (rounded)\n", LDBL_DIG, LDBL_MIN);
	printf("LDBL_MIN      = %1.*Le\n", my_LDBL_DECIMAL_DIG, LDBL_MIN);
	printf("LDBL_MIN      ='%s'\n", ___STRING(LDBL_MIN));

	putchar('\n');

	printf("LDBL_MAX      = %La\n", LDBL_MAX);
	printf("LDBL_MAX      = %1.*Le (rounded)\n", LDBL_DIG, LDBL_MAX);
	printf("LDBL_MAX      = %1.*Le\n", my_LDBL_DECIMAL_DIG, LDBL_MAX);
	printf("LDBL_MAX      ='%s'\n", ___STRING(LDBL_MAX));

	putchar('\n');

	printf("LDBL_EPSILON  = %La\n", LDBL_EPSILON);
	printf("LDBL_EPSILON  = %1.*Le (rounded)\n", LDBL_DIG, LDBL_EPSILON);
	printf("LDBL_EPSILON  = %1.*Le\n", my_LDBL_DECIMAL_DIG, LDBL_EPSILON);
	printf("LDBL_EPSILON  ='%s'\n", ___STRING(LDBL_EPSILON));
	/* xxx should check these.... */
	printf("LDBL_MIN_EXP     = %d\n", LDBL_MIN_EXP);
	printf("LDBL_MAX_EXP     = %d\n", LDBL_MAX_EXP);
	printf("LDBL_MIN_10_EXP  = %d\n", LDBL_MIN_10_EXP);
#if defined(HAVE_FLOORL) && defined(HAVE_LOG10L) && defined(HAVE_LRINTL)
	printf("LDBL_MIN_10_EXP  = %d [calculated]\n", (int) lrintl(floorl(log10l(fabsl(LDBL_MIN)))) + 1);	/* xxx explain the +1 */
#endif
	printf("LDBL_MAX_10_EXP  = %d\n", LDBL_MAX_10_EXP);
#if defined(HAVE_FLOORL) && defined(HAVE_LOG10L) && defined(HAVE_LRINTL)
	printf("LDBL_MAX_10_EXP  = %d [calculated]\n", (int) lrintl(floorl(log10l(fabsl(LDBL_MAX)))));
#endif

	putchar('\n');

	printf("FLT_ROUNDS      = %d\n", FLT_ROUNDS);
#ifdef FLT_EVAL_METHOD
	printf("FLT_EVAL_METHOD = %d\n", FLT_EVAL_METHOD);
#endif
#ifdef FLT_HAS_SUBNORM
	printf("FLT_HAS_SUBNORM = %d\n", FLT_HAS_SUBNORM);
#endif


	return;
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " LDLIBS=-lm CSTD=c99 " fn " && ./" fn))
 * End:
 */
