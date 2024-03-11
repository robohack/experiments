#include <sys/cdefs.h>

#include <sys/types.h>

#include <sys/resource.h>
#include <sys/time.h>
#include <err.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>

/*
 * Counting bits set in an integer (i.e. calculating the integer's Hamming
 * Weight) as an excercise in benchmarking small algorithms using getrusage(2)
 *
 * for info about the worker algorithms used here see these pages:
 * <URL:http://graphics.stanford.edu/~seander/bithacks.html>
 * <URL:http://gurmeet.net/puzzles/fast-bit-counting-routines/>
 */

/*
 * WARNING:  time can appear to have gone backwards with getrusage(2)!
 *
 * See NetBSD Problem Report #30115 (and PR#10201).
 * See FreeBSD Problem Report #975 (and PR#10402).
 *
 * Problem has existed in all *BSDs since 4.4BSD if not earlier.
 *
 * Only FreeBSD has implemented a "fix" (as of rev.1.45 (svn r44725) of
 * kern_resource.c (etc.) on April 13, 1999)
 *
 * But maybe it is even worse than that -- distribution of time between user
 * and system doesn't seem to match reality!
 *
 * See the GNU MP Library (GMP)'s tune/time.c code for better timing?
 */

/*
 * do nothing much, but make sure you do it!
 */
unsigned int nullfunc(unsigned long);
unsigned int
nullfunc(unsigned long val)
{
	return (unsigned int) val;
}

/*
 * return the number of bits set to one in a value
 *
 * K&R, 1st Ed. 1978, page 47
 *
 * The only optimization here is to cut the loop short when all the remaining
 * high-order bits (that were not shifted in) are zeros.
 */
unsigned int bitcount(unsigned long);
unsigned int
bitcount(unsigned long val)
{
	unsigned int bc;

	for (bc = 0; val != 0; val >>= 1) {
		if  (val & 01) {
			bc++;
		}
	}

	return bc;
}

/*
 * return the number of bits set to one in a value
 *
 * K&R another way -- most compilers should generate the exact same code
 */
unsigned int bitcount2(unsigned long);
unsigned int
bitcount2(unsigned long val)
{
	unsigned int bc = 0;

	while (val) {
		bc += (unsigned int) (val & 01);
		val >>= 1;
	}

	return bc;
}

/*
 * return the number of bits set to one in a value
 *
 * Subtraction of 1 from a number toggles all the bits (from right to left) up
 * to and including the righmost set bit.
 *
 * So, if we decrement a number by 1 and do a bitwise and (&) with itself
 * ((n-1) & n), we will clear the righmost set bit in the number.
 *
 * Therefore if we do this in a loop until we're left with zero and count the
 * number of iterations then we get the count of set bits.
 *
 * Executes in O(n) operations where n is number bits set to one in a given
 * integer value.
 *
 * From http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetKernighan
 *
 *	Brian Kernighan's method goes through as many iterations as there are
 *	set bits.  So if we have a 32-bit word with only the high bit set, then
 *	it will only go once through the loop.
 *
 *	Published in 1978, in the C Programming Language 2nd Edition. (by Brian
 *	W. Kernighan and Dennis M. Ritchie) mentions this in exercise 2-9.  On
 *	April 19, 2006 Don Knuth pointed out to me that this method "was first
 *	published by Peter Wegner in CACM Vol 3 (1960), No 5, p 322.  (Also
 *	discovered independently by Derrick Lehmer and published in 1964 in a
 *	book edited by Beckenbach.)"
 */
unsigned int countbits_sparse(unsigned long);
unsigned int
countbits_sparse(unsigned long val)
{
	unsigned int bc = 0;

	while (val) {
		val &= val - 1;	/* clear the least significant bit set to one */
		bc++;
	}

	return bc;
}

/*
 * another way -- most compilers should generate the exact same code
 */
unsigned int countbits_sparse2(unsigned long);
unsigned int
countbits_sparse2(unsigned long val)
{
	unsigned int bc;	/* bc accumulates the # of bits set in v */

	for (bc = 0; val; bc++) {
		val &= val - 1;	/* clear the least significant bit that is set */
	}

	return bc;
}

/*
 * optimized, in theory, for cases where most bits are set
 *
 * Executes in O(n) operations where n is number bits set to zero in a given
 * integer value.
 */
unsigned int countbits_dense(unsigned long);
unsigned int
countbits_dense(unsigned long val)
{
	unsigned int bc = sizeof(val) * CHAR_BIT;

	for (val ^= ~0UL; val; bc--) {
		val &= val - 1;	/* clear the least significant bit that is set */
	}

	return bc;
}

/*
 * most efficient non-lookup variant from the URL above....
 */
#define COUNT_BITS(T, x, c)							\
	{									\
		T n = (x);							\
										\
		n = n - ((n >> 1) & ~((T) 0) / 3);				\
		n = (n & ~((T) 0) / 15 * 3) + ((n >> 2) & ~((T) 0) / 15 * 3);	\
		n = (n + (n >> 4)) & ~((T) 0) / 255 * 15;			\
		c = (unsigned int) ((n * (~((T) 0) / 255)) >> ((sizeof(T) - 1) * CHAR_BIT)); \
	}									\


unsigned int count_bits(unsigned long);
unsigned int
count_bits(unsigned long val)
{
	unsigned int bc;

	COUNT_BITS(unsigned long, val, bc)

	return bc;
}


/*
 * Unattributed quote apparently by Don Gillies:
 *
 * I wrote a fast bitcount macro for RISC machines in about 1990[[!?!?!?!?]].
 * It does not use advanced arithmetic (multiplication, division, %), memory
 * fetches (way too slow), branches (way too slow), but it does assume the CPU
 * has a 32-bit barrel shifter (in other words, >> 1 and >> 32 take the same
 * amount of cycles.) It assumes that small constants (such as 6, 12, 24) cost
 * nothing to load into the registers, or are stored in temporaries and reused
 * over and over again.
 *
 * With these assumptions, it counts 32 bits in about 16 cycles/instructions on
 * most RISC machines. Note that 15 instructions/cycles is close to a lower
 * bound on the number of cycles or instructions, because it seems to take at
 * least 3 instructions (mask, shift, operator) to cut the number of addends in
 * half, so log_2(32) = 5, 5 x 3 = 15 instructions is a quasi-lowerbound.
 *
 * Here is a secret to the first and most complex step:
 *
 * input output
 * AB    CD             Note
 * 00    00             = AB
 * 01    01             = AB
 * 10    01             = AB - (A >> 1) & 0x1
 * 11    10             = AB - (A >> 1) & 0x1
 *
 * so if I take the 1st column (A) above, shift it right 1 bit, and subtract it
 * from AB, I get the output (CD). The extension to 3 bits is similar; you can
 * check it with an 8-row boolean table like mine above if you wish.
 *
 * Don Gillies
 *
 * Newsgroups: comp.compression.research,comp.arch,comp.lang.c
 * Path: mips!sdd.hp.com!ux1.cso.uiuc.edu!m.cs.uiuc.edu!gillies
 * From: gillies@m.cs.uiuc.edu (Don Gillies)
 * Subject: Re: Algorithm for finding set bits in a bit-string (and MSB).
 * Message-ID: <1992Aug3.164044.28037@m.cs.uiuc.edu>
 * Organization: University of Illinois, Dept. of Comp. Sci., Urbana, IL
 * References: <1992Jul26.010325.9885@infonode.ingr.com>
 *	<MOSS.92Jul25222337@ibis.cs.umass.edu>
 *	<Bs3yBx.7AA@watserv1.uwaterloo.ca>
 *	<Bs46JC.35n@watcgl.uwaterloo.ca>
 * Date: Mon, 3 Aug 1992 16:40:44 GMT
 * Lines: 30
 *
 * What about using this algorithm to find the lowest bit set in a word
 *
 *         if (p & (p-1)) return(-1);      (* returns -1 if > 1 bit set *)
 *         return(bitcount(p ^ (p-1)));    (* use XOR, returns 32 if no bit set *)
 *
 * where bitcount counts the number of bits in a word?  bitcount exists
 * in the hardware of several cdc machines and elsewhere.  On other
 * machines, it can be implemented in 16 arithmetic ops or much less if
 * the computer has a fast % (remainder) operation.  For instance, on the
 * 68020 % is slow enough that the arithmetic bitcount() subroutine is
 * faster.  In fact, arithmetic bitcount can run faster than 8-bit
 * character lookups (assuming a memory lookup is 2 cycles, whereas an
 * arithmetic op is one cycle).  Table lookups only win with 16-bit
 * tables:
 *
 * bitcount(n)
 * long unsigned n;
 * {
 *	long unsigned tmp;
 *	tmp = n - ((n >> 1) & 033333333333) - ((n >> 2) & 011111111111);
 *      tmp = ((tmp + (tmp >> 3)) & 030707070707);
 *	tmp =  (tmp + (tmp >> 6));
 *	tmp = (tmp + (tmp >> 12) + (tmp >> 24)) & 077;
 *			(* 6+3+3+5 = 16 ops *)
 * --
 *
 * Also:
 *
 * According to Donald Knuth ("The Art of Computer Programming", Vol IV, p 11),
 * the code first appeared in the first textbook on programming, "The
 * Preparation of Programs for an Electronic Digital Computer" by Maurice
 * Vincent Wilkes, David J. Wheeler, and Stanley Gill (Addison-Wesley Press, 2nd
 * Edition, 1957).
 *
 * Pages 191–193 of the book presented Nifty Parallel Count by D B Gillies and
 * J C P Miller.
 */
#define BitCount(X, Y)						\
	(Y = X - ((X >> 1) & 033333333333UL)		\
	       - ((X >> 2) & 011111111111UL),		\
	 Y = ((Y + (Y >> 3)) & 030707070707UL),		\
	 Y =  (Y + (Y >> 6)),			\
	 Y = (Y + (Y >> 12) + (Y >> 24)) & 077UL)

unsigned int count_bits_risc(unsigned long);
unsigned int
count_bits_risc(unsigned long val)
{
	unsigned long bc;

	BitCount(val, bc);

	return (unsigned int) bc;
}


/*
 * fortune(6):	-- really weird C code to count the number of bits in a word
 *
 * a variation on Gillies & Miller's algorithm?
 */

#define FORTUNE_BITCOUNT(x)	(((BX_(x) + (BX_(x) >> 4)) & 0x0F0F0F0FUL) % 255)
#define BX_(x)			((x) - (((x) >> 1) & 0x77777777UL)		\
				     - (((x) >> 2) & 0x33333333UL)		\
				     - (((x) >> 3) & 0x11111111UL))

unsigned int count_bits_fortune(unsigned long);
unsigned int
count_bits_fortune(unsigned long val)
{
	unsigned long bc;

	bc = FORTUNE_BITCOUNT(val);

	return (unsigned int) bc;
}


/*
 * Parallel Count carries out bit counting in a parallel fashion.  Consider n
 * after the first line has finished executing.  Imagine splitting n into pairs
 * of bits.  Each pair contains the "number of ones" in those two bit positions
 * in the original n.  After the second line has finished executing, each
 * nibble contains the "number of ones" in those four bits positions in the
 * original n.  Continuing this for five iterations, the 64 bits contain the
 * number of ones among these sixty-four bit positions in the original n.  That
 * is what we wanted to compute.
 */

#define TWO(c)		(0x1u << (c))
#define MASK(c)		(((unsigned int) (-1)) / (TWO(TWO(c)) + 1u))
#define COUNT(x,c)	((x) & MASK(c)) + (((x) >> (TWO(c))) & MASK(c))

unsigned int parallel_bitcount(unsigned long);
unsigned int
parallel_bitcount(unsigned long val)
{
	val = COUNT(val, 0);
	val = COUNT(val, 1);
	val = COUNT(val, 2);
	val = COUNT(val, 3);
	val = COUNT(val, 4);

	/* val = COUNT(val, 5) ;    for 64-bit integers */

	return (unsigned int) val;
}


/*
 * Nifty Parallel Count works the same way as Parallel Count for the first
 * three iterations. At the end of the third line (just before the return),
 * each byte of n contains the number of ones in those eight bit positions in
 * the original n.  A little thought experiment then explains why the remainder
 * modulo 255 works.
 */

#define MASK_01010101	(((unsigned int) (-1)) / 3)
#define MASK_00110011	(((unsigned int) (-1)) / 5)
#define MASK_00001111	(((unsigned int) (-1)) / 17)

unsigned int nifty_bitcount(unsigned long);
unsigned int
nifty_bitcount(unsigned long val)
{
	val = (val & MASK_01010101) + ((val >> 1) & MASK_01010101);
	val = (val & MASK_00110011) + ((val >> 2) & MASK_00110011);
	val = (val & MASK_00001111) + ((val >> 4) & MASK_00001111);

	return (unsigned int) (val % 255UL);
}

/*
 * MIT Bitcount
 *
 * Consider a 3 bit number as being
 *      4a+2b+c
 * if we shift it right 1 bit, we have
 *      2a+b
 * subtracting this from the original gives
 *      2a+b+c
 * if we shift the original 2 bits right we get
 *      a
 * and so with another subtraction we have
 *      a+b+c
 * which is the number of bits in the original number.
 *
 * Suitable masking allows the sums of the octal digits in a 32 bit number to
 * appear in each octal digit.  This isn't much help unless we can get all of
 * them summed together.  This can be done by modulo arithmetic (sum the digits
 * in a number by molulo the base of the number minus one) the old "casting out
 * nines" trick they taught in school before calculators were invented.  Now,
 * using mod 7 wont help us, because our number will very likely have more than
 * 7 bits set.  So add the octal digits together to get base64 digits, and use
 * modulo 63.  (Those of you with 64 bit machines need to add 3 octal digits
 * together to get base512 digits, and use mod 511.)
 *
 * This is HACKMEM 169, as used in X11 sources.
 * Source: MIT AI Lab memo, late 1970's.
 *
 * works for 32-bit numbers only
 */
unsigned int mit_bitcount(unsigned long);
unsigned int
mit_bitcount(unsigned long val)
{
	register unsigned long tmp;

	tmp = val - ((val >> 1) & 033333333333UL)
	          - ((val >> 2) & 011111111111UL);

	return (unsigned int) (((tmp + (tmp >> 3)) & 030707070707UL) % 63UL); /* 63 = 077 = 0x3F */
}


static unsigned char _bitCountTable8[1 << CHAR_BIT];
static bool _lookupTable8_Initialized = false;

#define defaultBitCount		count_bits_risc

static void _initBitCountTable8(void);
static void
_initBitCountTable8(void)
{
	unsigned long i;

	if (_lookupTable8_Initialized) {
		return;
	}

	for (i = 1; i < (1 << CHAR_BIT); i++) {
#if 1
		_bitCountTable8[i] = (unsigned char) (_bitCountTable8[i >> 1] + (i & 1));
#else
		_bitCountTable8[i] = (unsigned char) defaultBitCount(i);
#endif
	}

	_lookupTable8_Initialized = true;

	return;
}

/*
 * Look up each byte (uchar-sized hunk) of the value in a lookup table.
 */
unsigned int table_8_BitCount(unsigned long);
unsigned int
table_8_BitCount(unsigned long val)
{
	unsigned int bc = 0;

#if 0
	if (!_lookupTable8_Initialized) {
		_initBitCountTable8();
	}
#endif

	/* here we terminate early if all that remains are zero-bytes */
	while (val) {
		bc += _bitCountTable8[val & UCHAR_MAX];
		val >>= CHAR_BIT;
	}

	return bc;
}


/*
 * unrolled version
 */
unsigned int table_8_BitCount2(unsigned long);
unsigned int
table_8_BitCount2(unsigned long val)
{
#if 0
	if (!_lookupTable8_Initialized) {
		_initBitCountTable8();
	}
#endif

	return (unsigned int) (_bitCountTable8[val                   & UCHAR_MAX] +
			       _bitCountTable8[(val >> CHAR_BIT)     & UCHAR_MAX] +
			       _bitCountTable8[(val >> CHAR_BIT * 2) & UCHAR_MAX] +
			       _bitCountTable8[(val >> CHAR_BIT * 3) & UCHAR_MAX]);
}

static unsigned char _bitCountTable16[(1U << 16) +1];
static bool _lookupTable16_Initialized = false;

static void _initBitCountTable16(void);
static void
_initBitCountTable16(void)
{
	unsigned long i;

	if (_lookupTable16_Initialized) {
		return;
	}

	for (i = 1; i <= (1U << 16); i++) {
#if 1
		_bitCountTable16[i] = (unsigned char) (_bitCountTable16[i >> 1] + (i & 1));
#else
		_bitCountTable16[i] = (unsigned char) defaultBitCount(i);
#endif
	}

	_lookupTable8_Initialized = true;

	return;
}

/*
 * Look up each byte (uchar-sized hunk) of the value in a lookup table.
 */
unsigned int table_16_BitCount(unsigned long);
unsigned int
table_16_BitCount(unsigned long val)
{
	unsigned int bc = 0;

#if 0
	if (!_lookupTable16_Initialized) {
		_initBitCountTable16();
	}
#endif

	/* here we terminate early if all that remains are zero-bytes */
	while (val) {
		bc += _bitCountTable16[val & UINT16_MAX];
		val >>= 16;
	}

	return bc;
}


/*
 * unrolled version
 */
unsigned int table_16_BitCount2(unsigned long);
unsigned int
table_16_BitCount2(unsigned long val)
{
#if 0
	if (!_lookupTable16_Initialized) {
		_initBitCountTable16();
	}
#endif

	return (unsigned int) (_bitCountTable16[val         & UINT16_MAX] +
			       _bitCountTable16[(val >> 16) & UINT16_MAX]);
}


#if 0
void
verify_bitcounts(unsigned long val)
{
	unsigned int iterated_ones, sparse_ones, dense_ones;
	unsigned int precomputed_ones, precomputed16_ones;
	unsigned int parallel_ones, nifty_ones;
	unsigned int mit_ones;

	iterated_ones      = iterated_bitcount      (val);
	sparse_ones        = sparse_ones_bitcount   (val);
	dense_ones         = dense_ones_bitcount    (val);
	precomputed_ones   = precomputed_bitcount   (val);
	precomputed16_ones = precomputed16_bitcount (val);
	parallel_ones      = parallel_bitcount      (val);
	nifty_ones         = nifty_bitcount         (val);
	mit_ones           = mit_bitcount           (val);

	if (iterated_ones != sparse_ones) {
		errx(1, "ERROR: sparse_bitcount (0x%x) not okay!\n", x);
	}
	if (iterated_ones != dense_ones) {
		errx(1, "ERROR: dense_bitcount (0x%x) not okay!\n", x);
	}
	if (iterated_ones != precomputed_ones) {
		errx(1, "ERROR: precomputed_bitcount (0x%x) not okay!\n", x);
	}
	if (iterated_ones != precomputed16_ones) {
		errx(1, "ERROR: precomputed16_bitcount (0x%x) not okay!\n", x);
	}
	if (iterated_ones != parallel_ones) {
		errx(1, "ERROR: parallel_bitcount (0x%x) not okay!\n", x);
	}
	if (iterated_ones != nifty_ones) {
		errx(1, "ERROR: nifty_bitcount (0x%x) not okay!\n", x);
	}
	if (mit_ones != nifty_ones) {
		errx(1, "ERROR: mit_bitcount (0x%x) not okay!\n", x);
	}

	return;
}
#endif

/* XXX see also timevalsub() */
suseconds_t difftval(struct timeval, struct timeval);
suseconds_t
difftval(struct timeval tstart, struct timeval tend)
{
	tend.tv_sec -= tstart.tv_sec;
	tend.tv_usec -= tstart.tv_usec;

	/*
	 * be extremely careful that any over/under "impossible" tv_usec
	 * values from the above subtractions or earlier are ironed out and it
	 * is always left in range.
	 *
	 * XXX instead of a loop we could/should use '/' and '%', if this is
	 * right:
	 *
	 *	if (tend.tv_usec < 0) {
	 *	 	tv_sec -= (abs(tv_usec) / 1000000) + 1;
	 *	 	tv_usec = 1000000 - (abs(tv_usec) % 1000000);
	 *	}
	 *	if (tend.tv_usec >= 1000000) {
	 *	 	tv_sec += tv_usec / 1000000;
	 *	 	tv_usec = tv_usec % 1000000;
	 *	}
	 */
	while (tend.tv_usec < 0) {
		tend.tv_sec--;
		tend.tv_usec += 1000000;
	}
	while (tend.tv_usec >= 1000000) {
		tend.tv_sec++;
		tend.tv_usec -= 1000000;
	}

	return (suseconds_t) ((tend.tv_sec * 1000000) + tend.tv_usec);
}

/*
 * Timing anomalies
 *
 * time(1) uses gettimeofday() to show the "real" time, by which it means the
 * wall-clock time it took to run the process, including the time to do the
 * vfork() and execvp(), ignore some signals, and call wait4().
 *
 * However currently on NetBSD because of the bogus way 4BSD has approximately
 * always divied up time between user time and system time we can see
 * getrusage() report a total of system plus user time of as much as 0.06
 * seconds longer than gettimeofay() says it took for the whole thing!  E.g.:
 *
 * $ /usr/bin/time -p false
 * real         0.00
 * user         0.03
 * sys          0.03
 *
 * Furthermore gettimeofday() can wander, e.g. due to NTP, or worse.
 *
 * Use the POSIX.1b-1993 clock_gettime(CLOCK_MONOTONIC, tspec) instead if possible!
 *
 * WARNING:  apparently the Linux folks mis-read the POSIX.1b specifications
 * and/or didn't understand the definition of a monotonically increasing time
 * clock, and their CLOCK_MONOTONIC clock is affected by system time
 * adjustments, so you have to use their invented non-standard
 * CLOCK_MONOTONIC_RAW clock to get a real monotonic time clock.  Note too that
 * some sources claim CLOCK_MONOTONIC_RAW sometiems produces garbage results,
 * and is also significantly more expensive to call [1].
 *
 * On the other hand note that CLOCK_MONOTONIC is only subject to incremental
 * corrections, not sudden jumps, so CLOCK_MONOTONIC_RAW would be relevant
 * mainly to cases where more accurate time is wanted over very short intervals,
 * and CLOCK_MONOTONIC would be preferable for longer-term timers measured in
 * minutes, hours or days.  Of couse we _are_ measuring short intervals here.
 *
 * XXX the rest of this is informational -- we really only want CLOCK_MONOTONIC
 * here, though CLOCK_PROCESS_CPUTIME_ID would be more accurate on multitasking
 * systems.
 *
 * POSIX.1b-1999 now says there's a CPT option taken from ISO C:
 *
 *	If _POSIX_CPUTIME is defined, implementations shall support clock ID
 *	values obtained by invoking clock_getcpuclockid(), which represent the
 *	CPU-time clock of a given process.  Implementations shall also support
 *	the special clockid_t value CLOCK_PROCESS_CPUTIME_ID, which represents
 *	the CPU-time clock of the calling process when invoking one of the
 *	clock_*() or timer_*() functions.  For these clock IDs, the values
 *	returned by clock_gettime() and specified by clock_settime() represent
 *	the amount of execution time of the process associated with the clock.
 *	Changing the value of a CPU-time clock via clock_settime() shall have
 *	no effect on the behavior of the sporadic server scheduling policy.
 *
 * (and similar for CLOCK_THREAD_CPUTIME_ID)
 *
 * FreeBSD has these, and also has something similar called CLOCK_PROF, which
 * presumably accounts for all non-idle (non-wait-io) CPU time.
 *
 * The Linux manual warns:
 *
 *	The CLOCK_PROCESS_CPUTIME_ID and CLOCK_THREAD_CPUTIME_ID clocks are
 *	realized on many platforms using timers from the CPUs (TSC on i386,
 *	AR.ITC on Itanium). These registers may differ between CPUs and as a
 *	consequence these clocks may return bogus results if a process is
 *	migrated to another CPU.
 *
 *	If the CPUs in an SMP system have different clock sources then there is
 *	no way to maintain a correlation between the timer registers since each
 *	CPU will run at a slightly different frequency. If that is the case
 *	then clock_getcpuclockid(0) will return ENOENT to signify this
 *	condition. The two clocks will then only be useful if it can be ensured
 *	that a process stays on a certain CPU.
 *
 *	The processors in an SMP system do not start all at exactly the same
 *	time and therefore the timer registers are typically running at an
 *	offset. Some architectures include code that attempts to limit these
 *	offsets on bootup. However, the code cannot guarantee to accurately
 *	tune the offsets. Glibc contains no provisions to deal with these
 *	offsets (unlike the Linux Kernel). Typically these offsets are small
 *	and therefore the effects may be negligible in most cases.
 *
 * The Linux (er, glibc) manual is not clear on whether the CPU TSC registers
 * are saved and restored on each context switch either and there are reports
 * that at least some kernel versions will count the time spent in sleep(3),
 * for example.
 *
 * Apparently Android made it even worse, according to this comment on StkOvf:
 *
 *	For Android users, using CLOCK_MONOTONIC may be problematic since the
 *	app may get suspended, along with the clock.  For that, Android added
 *	the ANDROID_ALARM_ELAPSED_REALTIME timer that is accessible through
 *	ioctl().  [[ Itay Bianco ]]
 *
 * Darwin/MacOS makes things even worse by repeating the Linux mistakes and then
 * also introduces the better performing but less accurate
 * CLOCK_MONOTONIC_RAW_APPROX:
 *
 *	like CLOCK_MONOTONIC_RAW, but reads a value cached by the system at
 *	context switch.  This can be read faster, but at a loss of accuracy as
 *	it may return values that are milliseconds old.
 *
 * Note that FreeBSD has similar CLOCK_*_FAST, e.g. CLOCK_MONOTONIC_FAST, to
 * improve performance but with the limitation of reducing accuracy to "one
 * timer tick".
 *
 * Apparently Linux also fails to adjust CLOCK_MONOTONIC by not necessarily
 * incrementing it while the system is asleep (suspended).  They apparently
 * invented CLOCK_BOOTTIME to work around this sillyness.
 *
 * Note: suseconds_t is for signed values of times in microseconds, and it was
 * first added to POSIX 1003.1 in System Interfaces and Headers, Issue 5
 * published in 1997.  It must be no greater in size than a long int.  Note too
 * that POSIX is a bit finicky in specifying that suseconds_t only needs to
 * hold integers in the range of [0, 1000000] implicitly limiting it to just
 * one second intervals.  However we will abuse it slightly and assume it is at
 * least 32-bits and so can give us at least 35 second intervals, which should
 * be long enough for all our tests?
 *
 * [1] see also:  http://btorpey.github.io/blog/2014/02/18/clock-sources-in-linux/
 */

#ifdef __APPLE__
# define BEST_CLOCK_ID			CLOCK_MONOTONIC
# define BEST_CLOCK_ID_NAME		__STRING(CLOCK_MONOTONIC)
#endif

#if !defined(BEST_CLOCK_ID)
# if defined(CLOCK_MONOTONIC)
#  define BEST_CLOCK_ID			CLOCK_MONOTONIC
#  define BEST_CLOCK_ID_NAME		__STRING(CLOCK_MONOTONIC)
# endif
#endif
/*
 * Note in the above: neither ___STRING(), nor __STRING() can work on
 * BEST_CLOCK_ID to show the intermediate macro's name -- it's all or nothing
 * on expansion of a nested macro definition.
 */

/*
 * microtime() - return number of microseconds since some epoch
 *
 * the particular epoch is irrelevant -- we just use the difference between two
 * of these samples taken sufficiently far appart enough that the resolution is
 * also relatively unimportant, though better than 1 second is expected....
 */
suseconds_t microtime(void);
static void check_clock_res(void);

#if defined(BEST_CLOCK_ID)


# ifdef __APPLE__
#  ifdef __MAC_OS_X_VERSION_MIN_REQUIRED
#    if __MAC_OS_X_VERSION_MIN_REQUIRED < 101200

/*
 * XXX this is for Darwin / Mac OS X prior to Mac OSX 10.12, which did not
 * implement the POSIX (IEEE Std 1003.1b-1993) clock_gettime() API.
 *
 * macOS 10.12 offers CLOCK_MONOTONIC_RAW with the same claims as Linux, but
 * perhaps it is only for Linux compatability and not really necessary.
 *
 * macOS 10.12 also offers CLOCK_MONOTONIC_RAW_APPROX which is apparently
 * implemented using COMMPAGE, so may also be up to milliseconds old.
 *
 * See also:  https://stackoverflow.com/a/21352348/816536
 * and:  https://developer.apple.com/library/content/qa/qa1398/_index.html
 */

#include <sys/time.h>
#include <sys/resource.h>
#include <mach/mach.h>
#include <mach/clock.h>
#include <mach/mach_time.h>
#include <errno.h>
#include <unistd.h>
#include <sched.h>

typedef enum {
	CLOCK_REALTIME,
	CLOCK_MONOTONIC,
	CLOCK_PROCESS_CPUTIME_ID,
	CLOCK_THREAD_CPUTIME_ID
} clockid_t;

static mach_timebase_info_data_t __clock_gettime_inf;

int clock_gettime(clockid_t, struct timespec *);
int
clock_gettime(clockid_t clk_id,
              struct timespec *tp)
{
	kern_return_t	ret;
	clock_serv_t	clk;
	clock_id_t	clk_serv_id;
	mach_timespec_t	tm;
	uint64_t	start, end, delta, nano;
	int		retval = -1;

	switch (clk_id) {
	case CLOCK_REALTIME:
	case CLOCK_MONOTONIC:
		/* XXX these are both the same! */
		/* XXX this is apparently very slow too */
		clk_serv_id = clk_id == CLOCK_REALTIME ? REALTIME_CLOCK : SYSTEM_CLOCK;
		if ((ret = host_get_clock_service(mach_host_self(), clk_serv_id, &clk)) == KERN_SUCCESS) {
			if ((ret = clock_get_time(clk, &tm)) == KERN_SUCCESS) {
				tp->tv_sec  = tm.tv_sec;
				tp->tv_nsec = tm.tv_nsec;
				retval = 0;
			}
		}
		if (KERN_SUCCESS != ret) {
			errno = EINVAL;
			retval = -1;
		}
		break;
	case CLOCK_PROCESS_CPUTIME_ID:
		/*
		 * XXX this is an _extremely_ bad hack, but there you go....
		 *
		 * this measures elapsed time in ticks
		 */
		start = mach_absolute_time();
		if (clk_id == CLOCK_PROCESS_CPUTIME_ID) {
			getpid();
		} else {
			sched_yield();
		}
		end = mach_absolute_time();
		delta = end - start;
		if (__clock_gettime_inf.denom == 0) {
			mach_timebase_info(&__clock_gettime_inf);
		}
		nano = delta * __clock_gettime_inf.numer / __clock_gettime_inf.denom;
		tp->tv_sec = (time_t) (nano * (uint64_t) 1e-9);
		tp->tv_nsec = (long) (nano - (uint64_t) (tp->tv_sec * (time_t) 1e9));
		retval = 0;
		break;
	case CLOCK_THREAD_CPUTIME_ID:
	default:
		errno = EINVAL;
		retval = -1;
	}

	return retval;
}

int clock_getres(clockid_t, struct timespec *);
int
clock_getres(clockid_t clk_id,
             struct timespec *tp)
{
	kern_return_t	ret;
	clock_serv_t	clk;
	clock_id_t	clk_serv_id;
	natural_t	attribute[4];
	int		retval = -1;

	tp->tv_sec = 0;
	tp->tv_nsec = 1;

	switch (clk_id) {
        case CLOCK_REALTIME:
        case CLOCK_MONOTONIC:
		/* XXX these are both the same! */
		clk_serv_id = clk_id == CLOCK_REALTIME ? REALTIME_CLOCK : SYSTEM_CLOCK;
		if ((ret = host_get_clock_service(mach_host_self(), clk_serv_id, &clk)) == KERN_SUCCESS) {
			mach_msg_type_number_t count;

			count = sizeof(attribute)/sizeof(natural_t);
			if ((ret = clock_get_attributes(clk, CLOCK_GET_TIME_RES, (clock_attr_t) attribute, &count )) == KERN_SUCCESS) {
				tp->tv_sec  = 0;
				tp->tv_nsec = attribute[0];
				retval = 0;
			}
		}
		if (KERN_SUCCESS != ret) {
			errno = EINVAL;
			retval = -1;
		}
		break;
	case CLOCK_PROCESS_CPUTIME_ID:
		if (__clock_gettime_inf.denom == 0) {
			mach_timebase_info(&__clock_gettime_inf);
		}
		tp->tv_sec = 0;
		tp->tv_nsec = __clock_gettime_inf.numer / __clock_gettime_inf.denom;
		retval = 0;
		break;
	case CLOCK_THREAD_CPUTIME_ID:
	default:
		errno = EINVAL;
		retval = -1;
	}

	return retval;
}

#   endif
#  endif

# endif /* __APPLE__ */

suseconds_t
microtime(void)
{
	struct timespec tsnow;

	(void) clock_gettime(CLOCK_MONOTONIC, &tsnow);

	return (suseconds_t) ((tsnow.tv_sec * 1000000) + (tsnow.tv_nsec / 1000));
}

static void
check_clock_res(void)
{
	struct timespec res;

	/* XXX "#ifdef CLOCK_PROCESS_CPUTIME_ID"??? */
	if (clock_getres(CLOCK_MONOTONIC, &res) == -1) {
		err(EXIT_FAILURE, "clock_getres(CLOCK_MONOTONIC)");
	}
	warnx("using %s timer with resolution: %ld s, %ld ns", BEST_CLOCK_ID_NAME, res.tv_sec, res.tv_nsec);
}

#else /* ! BEST_CLOCK_ID */

/*
 * XXX N.B.:  apparently on linux times(NULL) is fast and returns a clock_t
 * value of CLK_TKS since the epoch, but it is probably implemented using
 * gettimeofday() anyway... (note: times() is POSIX-1003.1-1990)
 *
 * Note that on OS X the gettimeofday() function is implemented in libc as a
 * wrapper to either the _commpage_gettimeofday() function, if available, or
 * the normal system call.  If using the COMMPAGE helper then gettimeofday()
 * simply returns the value stored in the COMMPAGE and thus can execute without
 * a context switch.
 *
 * On BSD times() is just implemented using getrusage() and gettimeofday().
 */

suseconds_t
microtime(void)
{
	struct timeval tvnow;

	(void) gettimeofday(&tvnow, (void *) NULL);

	return (suseconds_t) ((tvnow.tv_sec * 1000000) + tvnow.tv_usec);
}

static void
check_clock_res(void)
{
	warnx("using gettimeofday() timer with unkown resolution");
	return;
}

#endif /* BEST_CLOCK_ID */

void show_time(const char *, unsigned int, suseconds_t, suseconds_t, suseconds_t);
void
show_time(const char *fname,
          unsigned int iter,
          suseconds_t us_u,
          suseconds_t us_s,
          suseconds_t us_c)
{
	suseconds_t us_w = (us_c - (us_s + us_u));
	double pc_u = (double) us_u / (double) iter ;
	double pc_s = (double) us_s / (double) iter ;
	double pc_w = (double) us_w / (double) iter ;
	double pc_c = (double) us_c / (double) iter ;

	/*
	 * note in the calculation of us_w above that wall clock elapsed time
	 * (us_c) is expected to be longer than the sum of user (us_u) and
	 * system (us_s) time, and we will display the difference as "wait"
	 * time, suggesting the amount of time the process was waiting for the
	 * CPU (or I/O, but none is done here) (shown here per call)
	 */

	printf("%22s = %5.4f us/c user, %7.4f us/c sys, %5.4f us/c wait, %7.4f us/c wall\n",
	       fname,
	       pc_u,
	       pc_s,
	       pc_w,
	       pc_c);
}

extern char *optarg;
extern int optind;
extern int optopt;
extern int opterr;
extern int optreset;

long int e_opt_strtol(void);

long int
e_opt_strtol(void)
{
	long int ltmp;
	char *ep;

	/*
	 * extremely pedantic parameter evaluation
	 */
	errno = 0;
	ltmp = strtol(optarg, &ep, 0);
	if (ep == optarg) {
		err(EX_USAGE,
		    "-%c param of '%s' is not a valid number",
		    optopt,
		    optarg);
	}
	if (*ep) {
		err(EX_USAGE,
		    "-%c param of '%s' has unsupported trailing unit specification characters",
		    optopt,
		    optarg);
	}
	if (errno != 0) {
		err(EX_USAGE,
		    "-%c param of '%s' is not convertible: %s",
		    optopt,
		    optarg,
		    strerror(errno));
	}
	if (ltmp > INT_MAX) {		/* XXX only needed if sizeof(long) > sizeof(int) */
		err(EX_USAGE,
		    "-%c param of %ld is too large (must be <= %d)",
		    optopt,
		    ltmp,
		    INT_MAX);
	}
	if (ltmp < 1) {
		err(EX_USAGE,
		    "-%c param of %ld is too small (must be > 0)",
		    optopt,
		    ltmp);
	}

	return ltmp;
}

long long int e_opt_strtoll(void);

long long int
e_opt_strtoll(void)
{
	long long int lltmp;
	char *ep;

	errno = 0;
	lltmp = strtoll(optarg, &ep, 0);
	if (ep == optarg) {
		err(EX_USAGE,
		    "-%c param of '%s' is not a valid number",
		    optopt,
		    optarg);
	}
	if (*ep) {
		err(EX_USAGE,
		    "-%c param of '%s' has unsupported trailing unit specification characters",
		    optopt,
		    optarg);
	}
	if (errno != 0) {
		err(EX_USAGE,
		    "-%c param of '%s' is not convertible: %s",
		    optopt,
		    optarg,
		    strerror(errno));
	}
	if (lltmp > LONG_MAX) {
		err(EX_USAGE,
		    "-%c param of %lld is too large (must be <= %d)",
		    optopt,
		    lltmp,
		    INT_MAX);
	}
	if (lltmp < 1) {
		err(EX_USAGE,
		    "-%c param of %lld is too small (must be > 0)",
		    optopt,
		    lltmp);
	}

	return lltmp;
}

#if defined(__APPLE__)
# define MILLIONS	100		/* my mac is way faster!  :-) */
#else
# define MILLIONS	10
#endif

/*
 * XXX not all systems have the same type for srandom(3)'s parameter!
 */
#if defined(__APPLE__) && !__DARWIN_UNIX03
/*
 * srandom() comes from 4.2BSD where its parameter was the standard "unsigned",
 * but for some stupid reason Apple changed the API in what they call UNIX03,
 * which also conflates LP64 with _DARWIN_C_SOURCE, _XOPEN_SOURCE, and
 * _POSIX_C_SOURCE.  The manual page says the "legacy" API was "void
 * srandom(unsigned long)"
 */
typedef unsigned long srandom_seed_t;
#else
typedef unsigned int srandom_seed_t;
#endif

#define MAX_STRLEN_OCTAL(t)	((int) ((sizeof(t) * CHAR_BIT / 3) + 2))

#if !defined(__GNUC__) || (defined(__GNUC__) && defined(__STRICT_ANSI__))
# define typeof	__typeof__
#endif

const char *argv0 = "progname";
static void usage(unsigned int)	__dead;

int main(int, char *[]);
int
main(int argc,
     char *argv[])
{
	unsigned int iter = MILLIONS * 1000000UL;
	bool dotest = true;
	bool use_random = false;
	int ch;
	srandom_seed_t seed = (srandom_seed_t) time((time_t *) NULL);
	size_t i;
	volatile unsigned int c;
	struct rusage rus;
	struct rusage ruf;
	suseconds_t nulltime_u;
	suseconds_t nulltime_s;
	suseconds_t nulltime_w;
	suseconds_t timetime_u;
	suseconds_t timetime_s;
	suseconds_t timetime_w;
	suseconds_t totaltime_u;
	suseconds_t totaltime_s;
	suseconds_t totaltime_w;

	argv0 = (argv0 = strrchr(argv[0], '/')) ? argv0 + 1 : argv[0];

	optind = 1;             /* Start options parsing */
	opterr = 0;             /* I'll print my own errors! */
	while ((ch = getopt(argc, argv, ":hi:rs:t")) != -1) {
		switch (ch) {
		case 'i':
			iter = (unsigned int) e_opt_strtol() * 1000000U;
			break;

		case 'r':
			use_random = true;
			break;

		case 's':
			seed = (srandom_seed_t) e_opt_strtoll();
			break;

		case 't':
			dotest = false;
			break;

		case 'h':
			usage(iter);

 		case '?':
			warnx("unknown option -- '%c'", optopt);
			usage(iter);

		case ':':
			/*
			 * NOTE: a leading ':' in optstring causes getopt() to
			 * return a ':' when an option is missing its parameter.
			 */
			warnx("missing parameter for -%c", optopt);
			usage(iter);

		default:
			warnx("programming error, unhandled flag: %c", ch);
			abort();
		}
	}
	argc -= optind;
	argv += optind;

	if (argc) {
		usage(iter);
	}
	/* show that some of the "important" ones work.... */
	for (i = (dotest ? 1 : INT_MAX); i < (sizeof(unsigned long) * CHAR_BIT); i++) {
		unsigned long v = 1UL << i;

#if 0
		verify_bitcounts(v/2+1);
#endif
		COUNT_BITS(unsigned long, v/2+1, c)
		printf("%#-*lo (v/2+1)   = %d, %d, %d, %d, %d\n", MAX_STRLEN_OCTAL(typeof(v)),
		       v/2+1, countbits_sparse(v/2+1), countbits_dense(v/2+1), bitcount2(v/2+1), count_bits(v/2+1), c);
#if 0
		verify_bitcounts(v-2);
#endif
		COUNT_BITS(unsigned long, v-2, c)
		printf("%#-*lo (v-2)     = %d, %d, %d, %d, %d\n", MAX_STRLEN_OCTAL(typeof(v)),
		       v-2, countbits_sparse(v-2), countbits_dense(v-2), bitcount2(v-2), count_bits(v-2), c);
#if 0
		verify_bitcounts(v-1);
#endif
		COUNT_BITS(unsigned long, v-1, c)
		printf("%#-*lo (v-1)     = %d, %d, %d, %d, %d\n", MAX_STRLEN_OCTAL(typeof(v)),
		       v-1, countbits_sparse(v-1), countbits_dense(v-1), bitcount2(v-1), count_bits(v-1), c);
#if 0
		verify_bitcounts(v);
#endif
		COUNT_BITS(unsigned long, v, c)
		printf("%#-*lo (%2d bits) = %d, %d, %d, %d, %d\n", MAX_STRLEN_OCTAL(typeof(v)),
		       v, (int) i, countbits_sparse(v), countbits_dense(v), bitcount2(v), count_bits(v), c);
		puts("--------");
	}

	check_clock_res();

	_initBitCountTable8();
	_initBitCountTable16();

	warnx("now running each algorithm for %u iterations....", iter);

	/*
	 * We will see from below that on NetBSD (and others except maybe
	 * FreeBSD) getrusage() can report _RADICALLY_ different amounts of
	 * both _user_ time and system time for the exact same code to run!
	 *
	 * This is over and above the occasional appearance of one or the other
	 * of these times appearing to go backwards (since they are both
	 * calculated by dividing the amount of total run time between them
	 * based on the number statclock tick "hits" which occured during that
	 * runtime and based on whether the hit was taking while in kernel mode
	 * or in user mode (or interrupt mode).
	 */

#define START_CLOCKS(v)	do {						\
	v##_w = microtime();						\
	getrusage(RUSAGE_SELF, &rus);					\
} while (0)

#define STOP_CLOCKS(v)	do {						\
	getrusage(RUSAGE_SELF, &ruf);					\
	v##_w = microtime() - v##_w;					\
	v##_u = difftval(rus.ru_utime, ruf.ru_utime);			\
	v##_s = difftval(rus.ru_stime, ruf.ru_stime);			\
} while (0)

	if (!use_random) {
		/* time time() */
		START_CLOCKS(timetime);
		for (i = 0; i < iter; i++) {
			c = (unsigned int) time((time_t *) NULL);
		}
		STOP_CLOCKS(timetime);
		show_time("time()", iter, timetime_u, timetime_s, timetime_w);
	} else {
		srandom(seed);
		/* time random() */
		START_CLOCKS(timetime);
		for (i = 0; i < iter; i++) {
			c = (unsigned int) random();
		}
		STOP_CLOCKS(timetime);
		show_time("random()", iter, timetime_u, timetime_s, timetime_w);
	}
	/* time nullfunc() */
	START_CLOCKS(nulltime);
	for (i = 0; i < iter; i++) {
		c = nullfunc((unsigned long) (use_random ? random() : time((time_t *) NULL)));
	}
	STOP_CLOCKS(nulltime);
	show_time("nullfunc()", iter, nulltime_u, nulltime_s, nulltime_w);

	/*
	 * note: leave nulltime_* as sum of nullfunc() time and time()/random() time
	 *
	 * nullfunc() overhead is the difference between nulltime_* and timetime_*
	 */

	/* time countbits_sparse() */
	START_CLOCKS(totaltime);
	for (i = 0; i < iter; i++) {
		c = countbits_sparse((unsigned long) (use_random ? random() : time((time_t *) NULL)));
	}
	STOP_CLOCKS(totaltime);
	show_time("countbits_sparse()", iter, totaltime_u, totaltime_s, totaltime_w);

	/* time countbits_sparse2() */
	START_CLOCKS(totaltime);
	for (i = 0; i < iter; i++) {
		c = countbits_sparse2((unsigned long) (use_random ? random() : time((time_t *) NULL)));
	}
	STOP_CLOCKS(totaltime);
	show_time("countbits_sparse2()", iter, totaltime_u, totaltime_s, totaltime_w);

	/* time countbits_dense() */
	START_CLOCKS(totaltime);
	for (i = 0; i < iter; i++) {
		c = countbits_dense((unsigned long) (use_random ? random() : time((time_t *) NULL)));
	}
	STOP_CLOCKS(totaltime);
	show_time("countbits_dense()", iter, totaltime_u, totaltime_s, totaltime_w);

	/* time COUNT_BITS() */
	START_CLOCKS(totaltime);
	for (i = 0; i < iter; i++) {
		COUNT_BITS(unsigned long, (unsigned long) (use_random ? random() : time((time_t *) NULL)), c)
	}
	STOP_CLOCKS(totaltime);
	show_time("COUNT_BITS()", iter, totaltime_u, totaltime_s, totaltime_w);

	/* time count_bits() */
	START_CLOCKS(totaltime);
	for (i = 0; i < iter; i++) {
		c = count_bits((unsigned long) (use_random ? random() : time((time_t *) NULL)));
	}
	STOP_CLOCKS(totaltime);
	show_time("count_bits()", iter, totaltime_u, totaltime_s, totaltime_w);

	/* time count_bits_risc() */
	START_CLOCKS(totaltime);
	for (i = 0; i < iter; i++) {
		c = count_bits_risc((unsigned long) (use_random ? random() : time((time_t *) NULL)));
	}
	STOP_CLOCKS(totaltime);
	show_time("count_bits_risc()", iter, totaltime_u, totaltime_s, totaltime_w);

	/* time count_bits_fortune() */
	START_CLOCKS(totaltime);
	for (i = 0; i < iter; i++) {
		c = count_bits_fortune((unsigned long) (use_random ? random() : time((time_t *) NULL)));
	}
	STOP_CLOCKS(totaltime);
	show_time("count_bits_fortune()", iter, totaltime_u, totaltime_s, totaltime_w);

	/* time bitcount2() */
	START_CLOCKS(totaltime);
	for (i = 0; i < iter; i++) {
		c = bitcount2((unsigned long) (use_random ? random() : time((time_t *) NULL)));
	}
	STOP_CLOCKS(totaltime);
	show_time("bitcount2()", iter, totaltime_u, totaltime_s, totaltime_w);

	/* time bitcount() */
	START_CLOCKS(totaltime);
	for (i = 0; i < iter; i++) {
		c = bitcount((unsigned long) (use_random ? random() : time((time_t *) NULL)));
	}
	STOP_CLOCKS(totaltime);
	show_time("bitcount()", iter, totaltime_u, totaltime_s, totaltime_w);

	/* time table_8_BitCount() */
	START_CLOCKS(totaltime);
	for (i = 0; i < iter; i++) {
		c = table_8_BitCount((unsigned long) (use_random ? random() : time((time_t *) NULL)));
	}
	STOP_CLOCKS(totaltime);
	show_time("table_8_BitCount()", iter, totaltime_u, totaltime_s, totaltime_w);

	/* time table_8_BitCount2() */
	START_CLOCKS(totaltime);
	for (i = 0; i < iter; i++) {
		c = table_8_BitCount2((unsigned long) (use_random ? random() : time((time_t *) NULL)));
	}
	STOP_CLOCKS(totaltime);
	show_time("table_8_BitCount2()", iter, totaltime_u, totaltime_s, totaltime_w);

	/* time table_16_BitCount() */
	START_CLOCKS(totaltime);
	for (i = 0; i < iter; i++) {
		c = table_16_BitCount((unsigned long) (use_random ? random() : time((time_t *) NULL)));
	}
	STOP_CLOCKS(totaltime);
	show_time("table_16_BitCount()", iter, totaltime_u, totaltime_s, totaltime_w);

	/* time table_16_BitCount2() */
	START_CLOCKS(totaltime);
	for (i = 0; i < iter; i++) {
		c = table_16_BitCount2((unsigned long) (use_random ? random() : time((time_t *) NULL)));
	}
	STOP_CLOCKS(totaltime);
	show_time("table_16_BitCount2()", iter, totaltime_u, totaltime_s, totaltime_w);

#if 1
	/* time the compiler's int __builtin_popcountl(unsigned long x); */
	START_CLOCKS(totaltime);
	for (i = 0; i < iter; i++) {
		c = (unsigned int) __builtin_popcountl((unsigned long) (use_random ? random() : time((time_t *) NULL)));
	}
	STOP_CLOCKS(totaltime);
	show_time("__builtin_popcountl()", iter, totaltime_u, totaltime_s, totaltime_w);
#endif

	exit(0);
}

static void
usage(unsigned int iter)
{
	fprintf(stderr, "Usage: %s [-tr] [-s seed] [-i millions_of_iterations]\n", argv0);
	fprintf(stderr, "-r: use random() instead of time()\n");
	fprintf(stderr, "-s seed: specify value for srandom() instead of using time()\n");
	fprintf(stderr, "-t: don't run the verbose proof of concept test -- just do timing runs\n");
	fprintf(stderr, "(default iterations: %lu * 10^6)\n", iter / 1000000UL);
	exit(EX_USAGE);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
