#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

/*
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
 * The LCC 4.x Code-Generation Interface Manual by Microsoft for MIPS says that
 * for bit field layout effectively ignores any platform ABI requirements:
 *
 * 	bit numbers start at zero with the least significant bit.  This bit
 *	field representation does not depend on the target's endiannes; the
 *	same representation is used for both big and little endians.
 *
 * Apparently Microsoft C allocates bit fields opposite to byte endian-ness?
 *
 * The SH-4 Generic and C Specific ABI says:
 *
 *	In the little-endian ABI, bit-fields are allocated from right to left
 *	(least to most significant) within a storage unit. In the big-endian
 *	ABI, bit-fields are allocated from left to right (most to least
 *	significant) within a storage unit.
 *
 * The UNIX System V ABI for PowerPC says that the CPU may be used in either
 * Little Endian mode or Big Endian mode, and with respect to bit-fields:
 *
 *	Bit-fields are allocated from right to left (least to most significant)
 *	on Little-Endian implementations and from left to right (most to least
 *	significant) on Big-Endian implementations.
 *
 * For i386, the UNIX System V ABI says:
 *
 *	Bit-fields are allocated from right to left (least to most significant).
 *
 * The UNIX System V ABI for MIPS gives only one valid bit-field layout:
 *
 *	Bit-fields are allocated from left to right (most to least significant).
 *
 * The UNIX System V AMD64 ABI says:
 *
 *	 bit-fields are allocated from right to left
 *
 * The LINUX ABI for IA64 syas:
 *
 *	Bit-fields are allocated from right to left (least to most significant).
 *
 * For all Sun Sparc:
 *
 * 	The C compiler assigns bit fields from the high to the low end of an
 * 	int (most to least significant).
 *
 * However for all Sun systems we can get the official value directly:
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

# if defined(mips) && defined(__LCC__)	/* xxx not sure "mips" is correct */
#  define _BIT_FIELDS_LTOH
# endif

# if defined(__i386__) || defined(__i386) || defined(i386) ||		\
    defined(__alpha__) || defined(__alpha) || defined(alpha) ||		\
    defined(__arm__) || defined(__arm) || defined(arm)
#  define _BIT_FIELDS_LTOH
# endif

#endif /* !(_BIT_FIELDS_LTOH || _BIT_FIELDS_HTOL) */

#if 1 /* !(defined(_BIT_FIELDS_LTOH) || defined(_BIT_FIELDS_HTOL)) */
/*
 * If that didn't work then try using platform-specific compile-time endian
 * discovery is a last-ditch effort.  This will likely only work on hosted
 * implementations.
 */

/* newer GCC & Clang will already have these defined... */
# if !(defined(__LITTLE_ENDIAN__) || defined(__BIG_ENDIAN__))

#  if !(defined(__BYTE_ORDER__) &&					\
	defined(__ORDER_LITTLE_ENDIAN__) && defined(__ORDER_BIG_ENDIAN__))

#   if defined(linux) || defined(__linux__) ||				\
	defined (__gnu_linux__) || defined(__linux)
#    include <endian.h>
#   endif

#   if defined(BSD) ||							\
	defined(__NetBSD__) ||						\
	defined(__FreeBSD__) ||						\
	defined(__OpenBSD__) ||						\
	defined(__Darwin__) ||						\
	defined(__DragonFly__)
#    include <sys/param.h>		/* also <sys/types.h>??? */
#    include <machine/endian.h>
#   endif

#   ifdef __SCO__
#    include <sys/types.h>
#    include <sys/byteorder.h>
#   endif

#   ifdef __aix__
#    include <sys/machine.h>
#   endif

#   ifdef __hpux
#    include <machine/param.h>
#   endif

#   ifdef OS2
#    include <machine/endian.h>
#   endif

#   ifdef SVR4
#    if defined(NCR) || defined(Mips) || defined(__sgi)
#     include <sys/endian.h>
#    else
#     if !defined(sun)
#      include <sys/byteorder.h>
#     endif
#    endif
#   endif

/* the above #includes should define one of the following... */

#   if !defined(__ORDER_LITTLE_ENDIAN__) && defined(_LITTLE_ENDIAN)
#    define __ORDER_LITTLE_ENDIAN__	_LITTLE_ENDIAN
#   endif
#   if !defined(__ORDER_BIG_ENDIAN__) && defined(_BIG_ENDIAN)
#    define __ORDER_BIG_ENDIAN__	_BIG_ENDIAN
#   endif
#   if !defined(__BYTE_ORDER__) && defined(_BYTE_ORDER)
#    define __BYTE_ORDER__		_BYTE_ORDER
#   endif

#   if !defined(__ORDER_LITTLE_ENDIAN__) && defined(__LITTLE_ENDIAN)
#    define __ORDER_LITTLE_ENDIAN__	__LITTLE_ENDIAN
#   endif
#   if !defined(__ORDER_BIG_ENDIAN__) && defined(__BIG_ENDIAN)
#    define __ORDER_BIG_ENDIAN__	__BIG_ENDIAN
#   endif
#   if !defined(__BYTE_ORDER__) && defined(__BYTE_ORDER)
#    define __BYTE_ORDER__		__BYTE_ORDER
#   endif

/*
 * If we still dont know the target byte order, then we try one last effort.
 *
 * This is based on compiler CPU macros, so for bit-field determination it's no
 * better than those above, but for code wanting to know the word endianness...
 */
#   ifndef __BYTE_ORDER__

/*
 * note: these are the values GCC & Clang use....
 */
#    ifndef __ORDER_LITTLE_ENDIAN__
#     define __ORDER_LITTLE_ENDIAN__	1234
#    endif
#    ifndef __ORDER_BIG_ENDIAN__
#     define __ORDER_BIG_ENDIAN__   	4321
#    endif

#    if defined(__QNX__) && !defined(__QNXNTO__)
#     define __BYTE_ORDER__	__ORDER_LITTLE_ENDIAN__
#    endif  /* __QNX__ */

#    if defined(__QNXNTO__)
#     if defined(i386) || defined(__i386__) || defined(__x86__)
#      define __BYTE_ORDER__	__ORDER_LITTLE_ENDIAN__
#     else
#      define __BYTE_ORDER__	__ORDER_BIG_ENDIAN__
#     endif
#    endif  /* __QNXNTO__ */

#    ifdef Lynx
#     if defined(i386) || defined(__i386__) || defined(__x86__)
#      define __BYTE_ORDER__	__ORDER_LITTLE_ENDIAN__
#     else
#      define __BYTE_ORDER__	__ORDER_BIG_ENDIAN__
#     endif
#    endif  /* Lynx */

#    if !defined(__BYTE_ORDER__) && defined(_LITTLE_ENDIAN)
#     define __BYTE_ORDER__	__ORDER_LITTLE_ENDIAN__
#    endif
#    if !defined(__BYTE_ORDER__) && defined(_BIG_ENDIAN)
#     define __BYTE_ORDER__	__ORDER_BIG_ENDIAN__
#    endif

#   endif /* __BYTE_ORDER__ */

#  endif /* !(__BYTE_ORDER__ && __ORDER_LITTLE_ENDIAN__ && __ORDER_BIG_ENDIAN__)) */

#  if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#   define __BIG_ENDIAN__
#  elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#   define __LITTLE_ENDIAN__
#  endif

# endif /* !(__LITTLE_ENDIAN__) || __BIG_ENDIAN__) */

# if defined(__LITTLE_ENDIAN__)
#  define _BIT_FIELDS_LTOH
# elif defined(__BIG_ENDIAN__)
#  define _BIT_FIELS_HTOL
# endif

# if !(defined(_BIT_FIELDS_LTOH) || defined(_BIT_FIELDS_HTOL))
#  include "one of _BIT_FIELDS_LTOH or _BIT_FIELDS_HTOL must be defined"
# endif

#endif /* endian hacks */

/*
 * now a simple mnemonic name to represent the first bit's position....
 *
 * It must use the same ordinal numbering scheme as ffs(3),a nd this allows an
 * undefined value to be represented by 0.
 */
enum {
#ifdef _BIT_FIELDS_LTOH
	BITFIELDS_FIRST_BIT = 1
#elif defined(_BIT_FIELDS_HTOL)
	BITFIELDS_FIRST_BIT = WORD_BIT
#else
	BITFIELDS_FIRST_BIT = 0
#endif
};

union bitfield_test {
	struct a_bit {
		unsigned int first_bit:1;
		unsigned int other_bits:(WORD_BIT-2);
		unsigned int last_bit:1;
	} the_bits;
	unsigned int the_int;
};

static volatile uint32_t l = 0x01020304UL;
volatile uint8_t *p = (volatile uint8_t *) &l;

int main(void);
int
main()
{
	union bitfield_test tb;
	bool little_bit_first = false;
	bool big_bit_first = false;
	unsigned int i;

	assert(WORD_BIT == sizeof(unsigned int) * CHAR_BIT);

	tb.the_int = 0;
	tb.the_bits.first_bit = 1;

	putchar('\n');
	printf("first_bit = %d (int = %u, aka 0x%8.8x)\n",
	       ffs((int) tb.the_int), tb.the_int, tb.the_int);
	if (ffs((int) tb.the_int) != BITFIELDS_FIRST_BIT) {
		printf("Compile-time detection says it should be %d\n", BITFIELDS_FIRST_BIT);
	}
	little_bit_first = (ffs((int) tb.the_int) == 1);

	tb.the_int = 0;
	tb.the_bits.last_bit = 1;

	printf("last_bit = %d (int = %u, aka 0x%8.8x)\n",
	       ffs((int) tb.the_int), tb.the_int, tb.the_int);

	big_bit_first = (ffs((int) tb.the_int) == 1);

	if (little_bit_first && big_bit_first) {
		printf("Houston, we have a problem.... (PDP-11?)\n");
	}
	if (!little_bit_first && !big_bit_first) {
		printf("Houston, we have an even bigger problem....\n");
	}
	if (tb.the_bits.other_bits != 0) {
		printf("It's full of stars....\n");
	}

	printf("LSB = %d\n", ffs(1));
	i = 1U << (WORD_BIT - 1);
	printf("MSB = %d\n", ffs((int) i));

	if (little_bit_first) {
		printf("#define _BIT_FIELDS_LTOH\t1\n");
#ifndef _BIT_FIELDS_LTOH
		printf("Hmmm...  but that was _not_ seen at compile time!\n");
#endif
	} else if (big_bit_first) {
		printf("#define _BIT_FIELDS_HTOL\t1\n");
#ifndef _BIT_FIELDS_HTOL
		printf("Hmmm...  but that was _not_ seen at compile time!\n");
#endif
	}
	putchar('\n');
	switch (*p) {
	case 0x01:
		printf("BIG-Endian\n");
		if (little_bit_first) {
			printf("hmmmmm...  bits & bytes don't match!\n");
		}
#ifndef __BIG_ENDIAN__
		printf("Hmmm...  BIG-Endian _not_ detected at compile time!\n");
#endif
#ifdef __LITTLE_ENDIAN__
		printf("OH OH!  LITTLE-Endian detected at compile time!!!\n");
#endif
		break;
	case 0x04:
		printf("LITTLE-Endian\n");
		if (big_bit_first) {
			printf("hmmmmm...  bits & bytes don't match!\n");
		}
#ifndef __LITTLE_ENDIAN__
		printf("Hmmm... LITTLE-Endian _not_ detected at compile time!\n");
#endif
#ifdef __BIG_ENDIAN__
		printf("OH OH!  BIG-Endian detected at compile time!!!\n");
#endif
		break;
	case 0x02:
		printf("PDP-11????  Impossible?\n");
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
		printf("l[%i] = 0x%02x%s", i, *p++, i == sizeof(l)-1 ? "\n" : ", ");
	}

	exit(0);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
