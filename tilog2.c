#include <sys/cdefs.h>

#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <sys/param.h>
#if defined(__NetBSD_Prereq__)
# if __NetBSD_Prereq__(5, 0, 0)
#  include <sys/bitops.h>
#  define HAVE_SYS_BITOPS_H	1
# endif
#endif

#ifndef HAVE_SYS_BITOPS_H
/*
 * Find First Set functions
 */
# ifndef ffs32
static __inline int __unused
ffs32(uint32_t _n)
{
	int _v;

	if (!_n)
		return 0;

	_v = 1;
	if ((_n & 0x0000FFFFU) == 0) {
		_n >>= 16;
		_v += 16;
	}
	if ((_n & 0x000000FFU) == 0) {
		_n >>= 8;
		_v += 8;
	}
	if ((_n & 0x0000000FU) == 0) {
		_n >>= 4;
		_v += 4;
	}
	if ((_n & 0x00000003U) == 0) {
		_n >>= 2;
		_v += 2;
	}
	if ((_n & 0x00000001U) == 0) {
		_n >>= 1;
		_v += 1;
	}
	return _v;
}
# endif

# ifndef ffs64
static __inline int __unused
ffs64(uint64_t _n)
{
	int _v;

	if (!_n)
		return 0;

	_v = 1;
	if ((_n & 0x00000000FFFFFFFFULL) == 0) {
		_n >>= 32;
		_v += 32;
	}
	if ((_n & 0x000000000000FFFFULL) == 0) {
		_n >>= 16;
		_v += 16;
	}
	if ((_n & 0x00000000000000FFULL) == 0) {
		_n >>= 8;
		_v += 8;
	}
	if ((_n & 0x000000000000000FULL) == 0) {
		_n >>= 4;
		_v += 4;
	}
	if ((_n & 0x0000000000000003ULL) == 0) {
		_n >>= 2;
		_v += 2;
	}
	if ((_n & 0x0000000000000001ULL) == 0) {
		_n >>= 1;
		_v += 1;
	}
	return _v;
}
# endif

/*
 * Find Last Set functions
 */
# ifndef fls32
static __inline int __unused
fls32(uint32_t _n)
{
	int _v;

	if (!_n)
		return 0;

	_v = 32;
	if ((_n & 0xFFFF0000U) == 0) {
		_n <<= 16;
		_v -= 16;
	}
	if ((_n & 0xFF000000U) == 0) {
		_n <<= 8;
		_v -= 8;
	}
	if ((_n & 0xF0000000U) == 0) {
		_n <<= 4;
		_v -= 4;
	}
	if ((_n & 0xC0000000U) == 0) {
		_n <<= 2;
		_v -= 2;
	}
	if ((_n & 0x80000000U) == 0) {
		_n <<= 1;
		_v -= 1;
	}
	return _v;
}
# endif

# ifndef fls64
static __inline int __unused
fls64(uint64_t _n)
{
	int _v;

	if (!_n)
		return 0;

	_v = 64;
	if ((_n & 0xFFFFFFFF00000000ULL) == 0) {
		_n <<= 32;
		_v -= 32;
	}
	if ((_n & 0xFFFF000000000000ULL) == 0) {
		_n <<= 16;
		_v -= 16;
	}
	if ((_n & 0xFF00000000000000ULL) == 0) {
		_n <<= 8;
		_v -= 8;
	}
	if ((_n & 0xF000000000000000ULL) == 0) {
		_n <<= 4;
		_v -= 4;
	}
	if ((_n & 0xC000000000000000ULL) == 0) {
		_n <<= 2;
		_v -= 2;
	}
	if ((_n & 0x8000000000000000ULL) == 0) {
		_n <<= 1;
		_v -= 1;
	}
	return _v;
}
# endif

/*
 * Integer logarithm, returns -1 on error. Inspired by the linux
 * version written by David Howells.
 */
# define _ilog2_helper(_n, _x)	((_n) & (1ULL << (_x))) ? _x :
# define ilog2(_n) \
( \
	__builtin_constant_p(_n) ? ( \
	_ilog2_helper(_n, 63) \
	_ilog2_helper(_n, 62) \
	_ilog2_helper(_n, 61) \
	_ilog2_helper(_n, 60) \
	_ilog2_helper(_n, 59) \
	_ilog2_helper(_n, 58) \
	_ilog2_helper(_n, 57) \
	_ilog2_helper(_n, 56) \
	_ilog2_helper(_n, 55) \
	_ilog2_helper(_n, 54) \
	_ilog2_helper(_n, 53) \
	_ilog2_helper(_n, 52) \
	_ilog2_helper(_n, 51) \
	_ilog2_helper(_n, 50) \
	_ilog2_helper(_n, 49) \
	_ilog2_helper(_n, 48) \
	_ilog2_helper(_n, 47) \
	_ilog2_helper(_n, 46) \
	_ilog2_helper(_n, 45) \
	_ilog2_helper(_n, 44) \
	_ilog2_helper(_n, 43) \
	_ilog2_helper(_n, 42) \
	_ilog2_helper(_n, 41) \
	_ilog2_helper(_n, 40) \
	_ilog2_helper(_n, 39) \
	_ilog2_helper(_n, 38) \
	_ilog2_helper(_n, 37) \
	_ilog2_helper(_n, 36) \
	_ilog2_helper(_n, 35) \
	_ilog2_helper(_n, 34) \
	_ilog2_helper(_n, 33) \
	_ilog2_helper(_n, 32) \
	_ilog2_helper(_n, 31) \
	_ilog2_helper(_n, 30) \
	_ilog2_helper(_n, 29) \
	_ilog2_helper(_n, 28) \
	_ilog2_helper(_n, 27) \
	_ilog2_helper(_n, 26) \
	_ilog2_helper(_n, 25) \
	_ilog2_helper(_n, 24) \
	_ilog2_helper(_n, 23) \
	_ilog2_helper(_n, 22) \
	_ilog2_helper(_n, 21) \
	_ilog2_helper(_n, 20) \
	_ilog2_helper(_n, 19) \
	_ilog2_helper(_n, 18) \
	_ilog2_helper(_n, 17) \
	_ilog2_helper(_n, 16) \
	_ilog2_helper(_n, 15) \
	_ilog2_helper(_n, 14) \
	_ilog2_helper(_n, 13) \
	_ilog2_helper(_n, 12) \
	_ilog2_helper(_n, 11) \
	_ilog2_helper(_n, 10) \
	_ilog2_helper(_n,  9) \
	_ilog2_helper(_n,  8) \
	_ilog2_helper(_n,  7) \
	_ilog2_helper(_n,  6) \
	_ilog2_helper(_n,  5) \
	_ilog2_helper(_n,  4) \
	_ilog2_helper(_n,  3) \
	_ilog2_helper(_n,  2) \
	_ilog2_helper(_n,  1) \
	_ilog2_helper(_n,  0) \
	-1) : ((sizeof(_n) > 4 ? fls64((uint64_t) _n) : fls32((uint32_t) _n)) - 1) \
)

static __inline void
fast_divide32_prepare(uint32_t _div, uint32_t * __restrict _m,
    uint8_t *__restrict _s1, uint8_t *__restrict _s2)
{
	uint64_t _mt;
	int _l;

	_l = fls32(_div - 1);
	_mt = (uint64_t)(0x100000000ULL * ((1ULL << _l) - _div));
	*_m = (uint32_t)(_mt / _div + 1);
	*_s1 = (_l > 1) ? 1 : _l;
	*_s2 = (_l == 0) ? 0 : _l - 1;
}

/* ARGSUSED */
static __inline uint32_t
fast_divide32(uint32_t _v, uint32_t _div __unused, uint32_t _m, uint8_t _s1,
    uint8_t _s2)
{
	uint32_t _t;

	_t = (uint32_t)(((uint64_t)_v * _m) >> 32);
	return (_t + ((_v - _t) >> _s1)) >> _s2;
}

static __inline uint32_t
fast_remainder32(uint32_t _v, uint32_t _div, uint32_t _m, uint8_t _s1,
    uint8_t _s2)
{

	return _v - _div * fast_divide32(_v, _div, _m, _s1, _s2);
}

#endif

/* from the internets */
#define LOG2(x) (30-((x)<2)-((x)<4)-((x)<8)-((x)<16)				\
                 -((x)<32)-((x)<64)-((x)<128)-((x)<256)-((x)<512)-((x)<1024)	\
                 -((x)<2048)-((x)<4096)-((x)<8192)-!((x)>>14)-!((x)>>15)	\
                 -!((x)>>16)-!((x)>>17)-!((x)>>18)-!((x)>>19)-!((x)>>20)	\
                 -!((x)>>21)-!((x)>>22)-!((x)>>23)-!((x)>>24)-!((x)>>25)	\
                 -!((x)>>26)-!((x)>>27)-!((x)>>28)-!((x)>>29)-!((x)>>30))


/*
 * find the most significant bit set in an integer
 *
 * aka the "log base 2" of an integer
 *
 * "the obvious way"
 */
u_int intlog2(uintmax_t);

u_int
intlog2(uintmax_t v)
{
	unsigned int mb = 0;

	while (v >>= 1) { /* unroll for more speed... */
		mb++;
	}

        return mb;
}

/*
 * Most Significant 1 Bit (unrolled?):
 *
 * aka "log base 2"
 *
 * a "binary search" algorithm using shift and compare operations
 *
 * The "log base 2" of an integer is the same as the position of the highest bit
 * (or most significant bit) set, the MSB.
 */
int msb(uintmax_t);

int
msb(uintmax_t v)
{
	int b = 0;

	if (!v)
		return -1;

	/*
	 * xxx this gets "warning: left shift count >= width of type"; and
	 * "warning: comparison of unsigned expression >= 0 is always true"; and
	 * "warning: right shift count >= width of type"
	 *
	 * all for the highest step (normally step(64))
	 */
#define step(x)	if (v >= ((uintmax_t) 1) << x)		\
			b += x, v >>= x

	if (sizeof(uintmax_t) > 8)
		step(64);
	if (sizeof(uintmax_t) >= 8)
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

u_int msb_naive(uintmax_t);


/*
 * Another variant of intlog2()
 */
u_int
msb_naive(uintmax_t x)
{
	u_int mb;

	for (mb = 0; x; mb++) {
		x >>= 1;
	}

	return mb;
}



uint32_t just_msb32(uint32_t x);
uint64_t just_msb64(uint64_t x);

/*
 * Given a binary unsigned integer value x, the most significant 1 bit can be
 * computed using a SWAR algorithm that recursively "folds" the upper bits into
 * the lower bits.  This process yields a bit vector with the same most
 * significant 1 as x, but with all 1's also set below it.  So, a bitwise AND of
 * the original value with the complement of the "folded" value shifted down
 * (right) by one yields just the most significant bit being set,
 *
 * For a 32-bit value:
 */
uint32_t
just_msb32(uint32_t x)
{
        x |= (x >> 1);
        x |= (x >> 2);
        x |= (x >> 4);
        x |= (x >> 8);
        x |= (x >> 16);

        return (x & ~(x >> 1));
}

uint64_t
just_msb64(uint64_t v)
{
        v |= (v >> 1);
        v |= (v >> 2);
        v |= (v >> 4);
        v |= (v >> 8);
        v |= (v >> 16);
        v |= (v >> 32);

        return (v & ~(v >> 1));
}



uintmax_t just_lsb(uintmax_t);

/*
 * Leave just the Least Significant 1 Bit set:
 *
 * This can be useful for extracting the lowest set element of a bit set.
 */
uintmax_t
just_lsb(uintmax_t v)
{
#if 1
	/*
	 * Given a 2's complement binary integer value x, (x&-x) leaves just
	 * the least significant 1 bit set.  (This was pointed-out by Tom May.)
	 *
	 * The reason this works is that it is equivalent to (x & ((~x) + 1));
	 *
	 * any trailing zero bits in x become ones in ~x, adding 1 to that
	 * carries into the following bit, thus flipping it, and so a logical
	 * AND with the original x yields only the flipped bit... the original
	 * position of the least significant 1 bit.
	 */
	return (v & -v);
#else
	/*
	 * Alternatively, but perhaps with one more instruction to execute,
	 * since (x&(x-1)) is actually x stripped of its least significant 1
	 * bit, the least significant 1 bit is also (x^(x&(x-1))).
	 */
	return (v ^ (v & (v - 1)));
#endif
}

int lsb(uintmax_t);

int
lsb(uintmax_t v)
{
	return msb(just_lsb(v));
}

int ctz(uintmax_t);

/*
 * Count Trailing Zeros:
 *
 * (Equivalent to GCC's __builtin_ctz().)
 */
int
ctz(uintmax_t x)
{
	return lsb(x) - 1;
}

int bitScanReverse(uint64_t bb);

/*
 * De Bruijn Multiplication
 *
 * Kim Walisch suggested to use the parallel prefix fill for a MS1B separation
 * with the same De Bruijn multiplication and lookup as in his bitScanForward
 * routine with separated LS1B, with less instructions in 64-bit mode.  A log
 * base 2 method was already devised by Eric Cole on January 8, 2006, and shaved
 * off rounded up to one less than the next power of 2 by Mark Dickinson on
 * December 10, 2009, as published in Sean Eron Anderson's Bit Twiddling Hacks
 * for 32-bit integers.
 *
 * from http://chessprogramming.wikispaces.com/BitScan
 */
const int index64[64] = {
    0, 47,  1, 56, 48, 27,  2, 60,
   57, 49, 41, 37, 28, 16,  3, 61,
   54, 58, 35, 52, 50, 42, 21, 44,
   38, 32, 29, 23, 17, 11,  4, 62,
   46, 55, 26, 59, 40, 36, 15, 53,
   34, 51, 20, 43, 31, 22, 10, 45,
   25, 39, 14, 33, 19, 30,  9, 24,
   13, 18,  8, 12,  7,  6,  5, 63
};

/**
 * bitScanReverse
 * @authors Kim Walisch, Mark Dickinson
 * @param bb 64-bit value to scan
 * @precondition bb != 0
 * @return index (0..63) of most significant one bit
 */
int
bitScanReverse(uint64_t bb)
{
	const uint64_t debruijn64 = 0x03f79d71b4cb0a89ULL;

	assert (bb != 0);

	bb |= bb >> 1;
	bb |= bb >> 2;
	bb |= bb >> 4;
	bb |= bb >> 8;
	bb |= bb >> 16;
	bb |= bb >> 32;

	return index64[(bb * debruijn64) >> 58];
}


bool ispow2(uintmax_t);

bool
ispow2(uintmax_t v)
{
	/* xxx maybe zero shouldn't be a power of 2? */
	if (v == 0) {
		return false;
	}
	/*
	 * Here we zero the least significant '1' bit, then check to see if the
	 * result is zero.
	 *
	 * a one bit followed by any number of zero bits will result in a zero
	 * in the same position with all those trailing zeros flipped to ones
	 * when one is subtracted, thus if a bit-wise AND with the original
	 * value returns zero then only that single bit was turned on and thus
	 * it was a power of 2.  On the other hand if there were any more
	 * significant '1' bits than the LSB they will remain un-changed by both
	 * operations and the result will not be zero.
	 */
	return ((v & (v - 1)) == 0);
}

#define binary_fmt(x, zf) binary_fmtsz((uintmax_t) (x), zf, sizeof(x))

char *binary_fmtsz(uintmax_t x, int zf, size_t sz);

char *
binary_fmtsz(uintmax_t x,
             int zf,			/* natural promoted type of bool */
             size_t sz)			/* sizeof(actual_x) */
{
	static char str[(sizeof(uintmax_t) * CHAR_BIT) + 1];
	char *s = str + (sz * CHAR_BIT);

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


/* use  */
unsigned int fast_intlog2(uintmax_t);

unsigned int
fast_intlog2(uintmax_t v)
{
#ifndef __has_builtin
# define __has_builtin(x) 0  /* for compatibility */
#endif

#if __has_builtin(__builtin_clz)
	return ((sizeof(uintmax_t) * CHAR_BIT) - 1) ^ __builtin_clzll(v);
#else
	return (unsigned int) msb(v);
#endif
}

int main(void);

int
main()
{
	unsigned int ui;
	uint32_t ui32;
	uint64_t ui64;

	/*
	 * the awesome power of the number 42 base-10!
	 *
	 * (note that ffs(42^n) == n-1)
	 *
	 * Note also that:
	 */
	const char *word = "math";
	const char *cp;
	char alpha[] = "*abcdefghijklmnopqrstuvwxyz"; /* count from 1 */
	int answ = 0;

	for (cp = word; *cp; cp++) {
		answ += (int) (strchr(alpha, *cp) - alpha);
	}
	printf("\nGREETINGS!  '%s' is the answer (%d) to life, the universe, and everything!\n\n",
	       word, answ);
	/*
	 * and perhaps the "proof" is in the relation of 42^n and binary
	 * numbers, namely the position of the LSB equals the power.
	 */

	for (ui = 0, ui32 = 1; ui < 6; ui++, ui32 *= 42) {
		printf("42^%d=%10u 0x%08x: %s\n",
		       ui, ui32, ui32, binary_fmt(ui32, true));
	}
	putchar('\n');

#ifndef ilog2
	printf("\nSorry, your system doesn't seem to have a <sys/bitops.h>\n");
#else
	for (ui = 0, ui32 = 1; ui < 6; ui++, ui32 *= 42) {
		printf("42^%d=%10u 0x%08x: ffs32()=%d lsb()=%02d fls32()=%02d msb()=%02d ilog2()=%02d\n",
		       ui, ui32, ui32, ffs32(ui32), lsb((uintmax_t) ui32), fls32(ui32), msb((uintmax_t) ui32), ilog2(ui32));
		assert(ilog2((uintmax_t) ui32) == msb((uintmax_t) ui32));
		assert((unsigned int) ilog2((uintmax_t) ui32) == intlog2((uintmax_t) ui32));
	}
	putchar('\n');

	printf("MAX =%10u 0x%08x: ffs32()=%d lsb()=%02d fls32()=%02d msb()=%02d ilog2()=%02d\n",
	       UINT32_MAX, UINT32_MAX, ffs32(UINT32_MAX), lsb((uintmax_t) UINT32_MAX), fls32(UINT32_MAX), msb((uintmax_t) UINT32_MAX), ilog2(UINT32_MAX));

	putchar('\n');

	for (ui = 0, ui32 = 1; ui < 6; ui++, ui32 *= 42) {
		printf("42^%d=%10u 0x%08x: ffs32()=%d lsb()=%02d just_msb32()=0x%08x\n",
		       ui, ui32, ui32, ffs32(ui32), lsb((uintmax_t) ui32), just_msb32(ui32));
	}
	putchar('\n');

	for (ui = 0, ui64 = 1; ui <= 11; ui++, ui64 *= 42) {
		printf("42^%02d= 0x%016jx: %s\n",
		       ui, (uintmax_t) ui64, binary_fmt(ui64, true));
	}
	putchar('\n');

	for (ui = 0, ui64 = 1; ui <= 11; ui++, ui64 *= 42) {
		printf("42^%02d=%20ju 0x%016jx: ffs64()=%02d lsb()=%02d fls64()=%02d ilog2()=%02d\n",
		       ui, (uintmax_t) ui64, (uintmax_t) ui64, ffs64(ui64), lsb(ui64), fls64(ui64), ilog2(ui64));
		assert(ilog2((uintmax_t) ui64) == msb((uintmax_t) ui64));
		assert((u_int) ilog2((uintmax_t) ui64) == intlog2((uintmax_t) ui64));
	}
	putchar('\n');

	printf("MAX  =%20ju 0x%016jx: ffs64()=%02d lsb()=%02d fls64()=%02d ilog2()=%02d\n",
	       (uintmax_t) UINT64_MAX, (uintmax_t) UINT64_MAX, ffs64(UINT64_MAX), lsb(UINT64_MAX), fls64(UINT64_MAX), ilog2(UINT64_MAX));

	putchar('\n');

	for (ui = 0, ui64 = 1; ui <= 11; ui++, ui64 *= 42) {
		printf("42^%02d=%20ju 0x%016jx: msb()=%02d, just_msb64=0x%016jx\n",
		       ui, (uintmax_t) ui64, (uintmax_t) ui64, bitScanReverse(ui64), (uintmax_t) just_msb64(ui64));
		assert(bitScanReverse(ui64) == msb(ui64));
	}
	putchar('\n');

	/*
	 * and 1302!
	 *
	 * (note again that ffs(1302^n) == n-1)
	 */

	for (ui = 0, ui64 = 1; ui <= 6; ui++, ui64 *= 1302) {
		printf("1302^%d=%20ju 0x%016jx: ffs64()=%d fls64()=%02d ilog2()=%02d\n",
		       ui, (uintmax_t) ui64, (uintmax_t) ui64, ffs64(ui64), fls64(ui64), ilog2(ui64));
	}
#endif
	putchar('\n');

#define TLOG2 fast_intlog2
	printf("LOG2(0<<0) = %02d%s\n", TLOG2(0LLU<<0), ispow2(0LLU<<0) ? "" : " FAILED: ispow2()!");
	for (ui = 0; ui < 64; ui++) {
		printf("LOG2(1<<%02d) = %d%s%s\n", ui, TLOG2(1LLU<<ui), TLOG2(1LLU<<ui) == ui ? "" : " FAILED: LOG2()", ispow2(1LLU<<ui) ? "" : " FAILED: ispow2()!");
	}

	exit(0);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
