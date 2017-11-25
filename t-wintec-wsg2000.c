/*
 *
 * Wintec WSG-2000 (G-Trender II) .NAL support.
 *
 * Copyright (C) 2012,2013  Greg A. Woods <woods@planix.ca>
 *
 * My normal copyright license is effectively a Creative Commons "Attribution
 * Share-Alike" license (with some specific terms about attribution and
 * re-distribution), but GPSbabel is GNU GPL.  Sigh.  I will also release this
 * under GPLv2 if I must.
 *
 * (On the other hand a CC "Attribution Share-Alike" copyright license is not
 * viral so code covered by it should be mixable with GPL?)
 */

#ifndef DO_MAIN
# include "defs.h"			/* i.e. from gpsbabel */
#endif
#include <assert.h>
#include <err.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
 * Note: a "compat/stdint.h" can be automatically generated, but doing so needs
 * help from code compiled for the target, but which can run on the build host.
 *
 * Note: a "compat/stdbool.h" need only contain these three lines:
 *
 *	typedef bool int;
 *
 *	# define false  0
 *	# define true   1
 */

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
#   include <sys/param.h>		/* also <sys/types.h>??? */
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

#if defined(__LITTLE_ENDIAN__) && defined(DO_MAIN)
# define le_assert(x)			assert(x)
#else
# define le_assert(x)			/* Nothing! */
#endif

#ifndef DO_MAIN
# define MYNAME "wsg2000"
#endif

#ifdef DO_MAIN /* for reference only! */
/*
 * Wintec WSG 2000 (G-Trender II) external record format
 *
 * The supplied M$-Windoze conversion and analysis software is a bad joke.
 *
 * File extension:	.NAL
 * File format:		binary, 32 byte fixed block length, little-endian,
 *			two's-complement, MSB
 *
 * Binary format analysis by: Dennis Heynlein (c-logic)
 * Posted at: <URL:http://www.mikrocontroller.net/topic/260568>
 *
 * Note that "named marks" are stored in the /SYS/MARK.BIN file on the device,
 * and not in the .NAL log files.  The "waypoint" flag simply indicates the
 * middle button on the left ("flag/arrow" icon, labeled "Push to Log / Start
 * New Log" in the manual) was pressed (twice).
 */
#pragma pack (1)
#if !defined(__LITTLE_ENDIAN__)
# error "This test code will not work on anything but little-endian machines.  Strip out the nal_t stuff for portable code"
#endif
#if UINT32_MAX < 0xffffffffU
# error "This test code will not work on any CPU with less than 32-bit integers."
#endif
/* XXX this probably won't work on compilers that don't allow all sized ints bitfields either */
typedef struct nal_data32_le {
	uint16_t start: 1, waypoint: 1, padding_flags: 14;

	/* note: the next two fit into one uint32_t */
	uint32_t second: 6, minute: 6, hour: 5; /* UTC(?) */
	uint32_t day: 5, month: 4, year: 6; /* add 2000 to year */

	int32_t latitude;		/* divide by 1E7 for degrees */
	int32_t longitude;		/* divide by 1E7 for degrees */

	uint16_t height;		/* meters (from altimiter?) */

	int8_t temperature;		/* °C */

	uint16_t pressure;		/* mbar */

	uint8_t cadence;		/* RPM from cadence sensor */
	uint8_t pulse;			/* BPM from heart rate sensor */

	int8_t slope;			/* degrees */

	int16_t compass;		/* ° around Z axis (unsigned?) */
	int16_t roll;			/* ° around X axis */
	int16_t yaw;			/* ° around Y axis ("pitch" in the Wintec manual) */

	uint8_t speed;			/* km/h (wheel speed sensor?) */

	uint8_t bike;			/* "user" ID# 0-3 */

	uint16_t padding_end;
} nal_t;
#endif /* DO_MAIN: for reference only! */

#define NAL_BLOCK_SZ	32		/* known value, sizeof(nal_t) must match */

/*
 * a GPS and related data point as logged by the Wintec WSG-2000
 *
 * This is an internal C-friendly representation of the data record.
 *
 * XXX perhaps [u]int8_t (or [unsigned] char) should be used for fields here
 * where the external format only supplies 8-bit values, and that would save us
 * from having to be extra careful about sign extension for signed 8-bit
 * conversions to short ints.  This would require [u]int8_t be a valid type....
 */
typedef struct gpspt {
	time_t		date;		/* record creation date */

	unsigned int	start :1;	/* start of track record */
	unsigned int	waypoint :1;	/* manually marked position */

	unsigned char	bike;		/* "user" ID# 0-3 */

	int		latitude;	/* divide by 1E7 for degrees */
	int		longitude;	/* divide by 1E7 for degrees */

	unsigned short	height;		/* meters (from altimiter?) */

	short		temperature;	/* °C [int8_t] */
	unsigned short	pressure;	/* mbar */

	unsigned short	cadence;	/* RPM from cadence sensor */
	unsigned short	pulse;		/* BPM from heart rate sensor */
	unsigned short	speed;		/* km/h (wheel speed sensor?) */

	short		slope;		/* degrees of incline (calculated?) [int8_t] */

	unsigned short	compass;	/* ° around Z axis */
	short		roll;		/* ° around X axis */
	short		yaw;		/* ° around Y axis */
} gpspt_t;


#ifdef DO_MAIN
int main(int, char *[]);

static size_t process(FILE *, const char *);
static bool is_file_nal_p(FILE *, const char *);
static void pp_nal(nal_t *, size_t);
#endif /* DO_MAIN */

static bool is_nal_p(gpspt_t, const char *, size_t);
static bool fill_gpspt(gpspt_t *pt, unsigned char *, const char *, size_t);
static int pp_gpspt(gpspt_t, size_t);
static void show_distance(int, int, int, int);
static void CalcDist2(int, int, int, int, double *, double *);

#ifdef DO_MAIN
int
main(int argc,
     char *argv[])
{
	size_t nr;
	size_t nt = 0;

	warnx("reading %lu byte records with %s byte order and %s bitfields.",
	      (unsigned long) sizeof(nal_t),
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
	assert(NAL_BLOCK_SZ == sizeof(nal_t));

	if (argc == 1) {
		if (!is_file_nal_p(stdin, "stdin")) {
			warnx("%s: does not seem to be NAL-format data", "stdin");
		} else {
			nr = process(stdin, "stdin");
			warnx("%s: read %lu records", "stdin", (unsigned long) nr);
		}
	} else {
		argc--;
		argv++;
		for (; argc > 0; --argc, argv++) {
			FILE *fin;
			size_t usum;

			if (!(fin = fopen(*argv, "r"))) {
				warn("error opening %s", *argv);
				continue;
			}
			if (!is_file_nal_p(fin, *argv)) {
				warnx("%s: does not seem to be NAL-format data", *argv);
				continue;
			}
			nr = process(fin, *argv);
			if (fclose(fin) != 0) {
				warn("%s: error closing", *argv);
			}
			warnx("%s: read %lu records", *argv, (unsigned long) nr);
			usum = nt + nr;
			if (usum < nt || usum < nr) {
				warnx("to many records to count in a %ld-sized integer", (unsigned long) sizeof(usum));
			}
			nt = usum;
		}
		warnx("read total of %lu records", (unsigned long) nt);
	}

	exit(0);
}

static size_t
process(FILE *fin,
	const char *fn)
{
	char buf[NAL_BLOCK_SZ];
	gpspt_t pt;
	size_t nb = 0;
	size_t nr = 0;

	while ((nb = fread(buf, (size_t) 1, sizeof(buf), fin)) == sizeof(buf)) {
		if (!fill_gpspt(&pt, (unsigned char *) buf, fn, nr)) {
			warnx("%s: record #%lu: failed to convert properly",
			      fn, (unsigned long) nr);
		}
#ifndef NDEBUG
		(void) is_nal_p(pt, fn, nr);
#endif
#ifdef DEBUG
		pp_nal((nal_t *) buf, nr);
#endif
		pp_gpspt(pt, nr);
		nr++;
	}
	if (nb > 0) {
		warnx("%s: record #%lu: read incomplete record of %lu bytes",
		      fn, (unsigned long) nr, (unsigned long) nb);
		return (0);
	}
	if (!feof(fin)) {
		warn("%s: finished reading before EOF (last record #%lu was %lu bytes short)",
		     fn, (unsigned long) nr, (unsigned long) (sizeof(buf) - nb));
	}
	if (ferror(fin)) {
		warn("%s: error reading after %lu records",
		     fn, (unsigned long) nr);
		return (0);
	}
	return nr;
}

static bool
is_file_nal_p(FILE *fin,
	      const char *fn)
{
	char buf[NAL_BLOCK_SZ];
	size_t nb;
	gpspt_t pt;
	bool rc;

	/*
	 * read the first record...
	 */
	if ((nb = fread(buf, (size_t) 1, sizeof(buf), fin)) != sizeof(buf)) {
		warn("%s: cannot read a complete %lu-byte record to verify format (read %lu bytes)",
		     fn, (unsigned long) sizeof(buf), (unsigned long) nb);
		return false;
	}

	/*
	 * try to convert it to internal form....
	 */
	if (!fill_gpspt(&pt, (unsigned char *) buf, fn, (size_t) 0)) {
		warnx("%s: failed to convert first record", fn);
		/* what did it look like???? */
		pp_nal((nal_t *) buf, (size_t) 0);
		return false;
	}

	/*
	 * check it out....
	 */
	rc = is_nal_p(pt, fn, (size_t) 0);

	/*
	 * and put it all back...
	 *
	 * (assume the default I/O buffering is full block buffering with a
	 * block size of BUFSIZ, as is normal for Unix and unix-like systems)
	 */
#if (BUFSIZ < NAL_BLOCK_SZ)
# error "Here be dragons!  Your stdio buffers are too small!  Need a setbuf()."
#endif
	for (; nb > 0; nb--) {
		/* note: allow the sign extension, then truncate back to sizeof(char) */
		if (ungetc((int) buf[nb - 1] & 0xFF, fin) == EOF) {
			warn("%s: ungetc()", fn); /* XXX perhaps this should be a fatal err()? */
			return false;
		}
	}

	return rc;
}
#endif	/* DO_MAIN */

static bool
is_nal_p(gpspt_t pt,
	 const char *fn,
	 size_t rn)
{
	if (abs(pt.latitude) > 90 * 10000000) {
		warnx("%s: #%lu: impossible latitude %d.%d°", fn, (unsigned long) rn, pt.latitude / 10000000,  pt.latitude % 10000000);
		return false;
	}
	if (abs(pt.longitude) > 180 * 10000000) {
		warnx("%s: #%lu: impossible longitude %d.%d°", fn, (unsigned long) rn, pt.longitude / 10000000, pt.longitude % 10000000);
		return false;
	}
	if (pt.latitude == 0 && pt.longitude == 0) {
		warnx("%s: #%lu: impossible position 0° 0° (unless you were on the equator in the middle of the Atlantic Ocean?)", fn, (unsigned long) rn);
		return false;
	}
	/*
	 * XXX not sure how this can be >90 either way, but I've seen +99° in
	 * one file, at a time when I was walking, possibly with the unit in my
	 * hand, and with my camera in the other, and while quite near to an
	 * iron pipe laid along the ground, sometimes walking, sometimes
	 * stopped....  all within the period of under two minutes.  I will
	 * have to revisit that location and see if there may be a magnetic
	 * anomaly or some other possible explanation.  Or perhaps a
	 * beyond-vertical slope up is possible?
	 *
	 * On the other hand given that compass, roll, and yaw probably come
	 * from the MEMS chip, and slope should be calculated by the GPS based
	 * on the delta of distance and height from the last point, or series
	 * of points?
	 */
	if (abs(pt.slope) > 180) {
		warnx("%s: #%lu: impossible slope %d°", fn, (unsigned long) rn, pt.slope);
		return false;
	}
	if (pt.compass > 360) {
		warnx("%s: #%lu: impossible compass %d°", fn, (unsigned long) rn, pt.compass);
		return false;
	}
	if (abs(pt.roll) > 180) {
		warnx("%s: #%lu: impossible roll %d°", fn, (unsigned long) rn, pt.roll);
		return false;
	}
	if (abs(pt.yaw) > 180) {
		warnx("%s: #%lu: impossible yaw %d°", fn, (unsigned long) rn, pt.yaw);
		return false;
	}
	/*
	 * XXX in theory we should verify that cadence, pulse, temperature, and
	 * speed are have values that will fit in an (unsigned) char, since
	 * that's how much data was read into them.
	 */

	return true;
}

/*
 * read a .NAL record (from "buf") into a gpspt_t structure (pointed at by
 * "pt"), using "fn" and "rn" in any error or warning messages as the file name
 * and record number which "buf" contains
 *
 * This code is endian-independent, but only independent of bitfield-order if
 * _BIT_FIELDS_LTOH is defined correctly for the compiler.
 *
 * Note that using unions for the purposes of memory reinterpretation
 * (i.e. writing one member and then reading another, aka type-punning) has
 * always been allowed by most C implementations (7th Edition Unix source did
 * this in several places).
 *
 * C99 was initially clumsily worded, appearing to make type-punning through
 * unions undefined.  In reality, type-punning though unions is legal in C89,
 * and legal in C11, and it was actually legal in C99 all along, however it took
 * until 2004 for the committee to fix the incorrect wording with the release
 * of C99-TC3.
 */
static bool
fill_gpspt(gpspt_t *pt,
	   unsigned char *buf,
	   const char *fn,
	   size_t rn)
{
	union flag_data {
		uint16_t val;
		struct bit_flags {
#ifdef _BIT_FIELDS_LTOH
			unsigned	start: 1, waypoint: 1, unknown_flags: 14;
#else
			unsigned	unknown_flags: 14, waypoint: 1, start: 1;
#endif
		} fl;
	} bfl = { 0 };
	union time_data {
		uint32_t val;
		struct bit_time {
			/* add 2000 to year */
#ifdef _BIT_FIELDS_LTOH
			unsigned	second: 6, minute: 6, hour: 5;
			unsigned	day: 5, month: 4, year: 6;
#else
			unsigned	year: 6, month: 4, day: 5;
			unsigned	hour: 5, minute: 6, second: 6;
#endif
		} tmi;
	} btm = { 0 };
	struct tm 	rt;
	unsigned int i = 0;

	/*
	 * Note: we probably don't have to worry about this since if "unsigned"
	 * isn't big enough for the first group of bits defined in struct
	 * bit_time (17), then hopefully the compiler will at least warn, if
	 * not error out entirely.
	 */
	assert(sizeof(btm.tmi) == sizeof(btm.val));

	memset((char *) pt, 0, sizeof(*pt));

	if (!pt) {
		return false;
	}
	if (!buf) {
		return false;
	}

	bfl.val = (uint16_t) (buf[i++]  << 0);
	bfl.val |= (uint16_t) (buf[i++] << 8);

	pt->start = bfl.fl.start;
	pt->waypoint = bfl.fl.waypoint;

	if (bfl.fl.unknown_flags != 0) {
		warnx("%s: #%lu: unknown flags: 0x%X", fn, (unsigned long) rn, (unsigned int) bfl.fl.unknown_flags);
		/* XXX should this be an error?  or do we care? */
	}

	btm.val = (uint32_t) (buf[i++]  << 0);
	btm.val |= (uint32_t) (buf[i++] << 8);
	btm.val |= (uint32_t) (buf[i++] << 16);
	btm.val |= (uint32_t) (buf[i++] << 24);

	/*
	 * Note: we check these bitfield values here so as to get more
	 * meaningful warnings, instead of just waiting for mktime() to fail.
	 *
	 * These are errors too because they indicate either bad data or some
	 * other kind of non-recoverable problem.
	 */
	if (btm.tmi.month == 0 || btm.tmi.month > 12) {
		warnx("%s: #%lu: impossible month %d", fn, (unsigned long) rn, btm.tmi.month);
		return false;
	}
	if (btm.tmi.day == 0 || btm.tmi.day > 31) {
		warnx("%s: #%lu: impossible day %d", fn, (unsigned long) rn, btm.tmi.day);
		return false;
	}
	if (btm.tmi.hour > 23) {
		warnx("%s: #%lu: impossible hour %d", fn, (unsigned long) rn, btm.tmi.hour);
		return false;
	}
	if (btm.tmi.minute > 60) {
		warnx("%s: #%lu: impossible minute %d", fn, (unsigned long) rn, btm.tmi.minute);
		return false;
	}
	if (btm.tmi.second > 60) {
		warnx("%s: #%lu: impossible second %d", fn, (unsigned long) rn, btm.tmi.second);
		return false;
	}
	rt.tm_sec = btm.tmi.second;
	rt.tm_min = btm.tmi.minute;
	rt.tm_hour = btm.tmi.hour;
	rt.tm_mday = btm.tmi.day;
	rt.tm_mon = btm.tmi.month - 1;
	rt.tm_year = btm.tmi.year + (2000 - 1900);
	rt.tm_gmtoff = 0;
	rt.tm_isdst = 0;
	rt.tm_zone = NULL;
	/*
	 * GPS time was zero at 1980/01/06-00:00:00Z and since GPS time is has
	 * not been perturbed by leap seconds the GPS time in seconds is ahead
	 * of UTC (which has had leap seconds inserted without being counted in
	 * seconds since the epoch).
	 *
	 * Strict Unix time also ignores leap seconds (the clock always ticks,
	 * and every second is counted).
	 *
	 * So, both Unix time and GPS time are similar to International Atomic
	 * Clock Time (TAI).
	 *
	 * On the other hand POSIX requires time_t to represent UTC.  Which was
	 * extremely stupid, but it's too late to change now, unless you just
	 * ignore it and stick to Unix (or TAI) time.  (It is easier this POSIX
	 * way to handle resetting the system clock after a reboot since
	 * civilian broadcast clocks do run on UTC.)
	 *
	 * The big question here is whether the Wintec folks are storing plain
	 * GPS time or not.....  Do they convert properly to UTC?  Perhaps if
	 * logging is on and the unit is connected and sending NMEA records to
	 * the USB port, then the time stamps from those records and those from
	 * the corresponding log can be compared with NTP time.
	 *
	 * On systems with the "tz" library it is possible to encode timezone
	 * files with leap-second information such that time_t values are given
	 * in TAI, in which case I think leap seconds will be inserted into the
	 * result by timegm(3), but that may not actually be what we want since
	 * GPS time is effectively in sync with TAI.  Using setenv("TZ", "", 1)
	 * may actually be best as that avoids any possible handling of leap
	 * seconds.
	 *
	 * XXX timegm(3) can be simulated on any known platform that offers a
	 * proper ISO C89 or POSIX mktime(3) implmentation with the following
	 * code (assuming setenv(3) and unsetenv(3), available since the
	 * Seventh Edition of Unix, is also available (which includes
	 * M$-Windoze all the way back to at least W95)):
	 *
	 *	char *otz = getenv("TZ");
	 * #ifdef WINDOZE
	 *	setenv("TZ", "UTC", 1);
	 * #else
	 *	setenv("TZ", "", 1);
	 * #endif
	 *	mktime(&rt);
	 *	if (otz)
	 *		setenv("TZ", otz, 1);
	 *	else
	 *		unsetenv("TZ");
	 */
	if ((pt->date = timegm(&rt)) == (time_t) -1) {
		warn("%s: #%lu: mktime(): failed to convert date", fn, (unsigned long) rn);
		return false;
	}

	le_assert(i == offsetof(nal_t, latitude));
	pt->latitude = (buf[i++]  << 0);
	pt->latitude |= (buf[i++] << 8);
	pt->latitude |= (buf[i++] << 16);
	pt->latitude |= (buf[i++] << 24);

	le_assert(i == offsetof(nal_t, longitude));
	pt->longitude = (buf[i++]  << 0);
	pt->longitude |= (buf[i++] << 8);
	pt->longitude |= (buf[i++] << 16);
	pt->longitude |= (buf[i++] << 24);

	le_assert(i == offsetof(nal_t, height));
	pt->height = (unsigned short) (buf[i++]  << 0);
	pt->height |= (unsigned short) (buf[i++] << 8);

	le_assert(i == offsetof(nal_t, temperature));
	pt->temperature = (signed char) buf[i++]; /* make sure the sign bit extends! */

	le_assert(i == offsetof(nal_t, pressure));
	pt->pressure = (unsigned short) (buf[i++]  << 0);
	pt->pressure |= (unsigned short) (buf[i++] << 8);

	le_assert(i == offsetof(nal_t, cadence));
	pt->cadence = buf[i++];

	le_assert(i == offsetof(nal_t, pulse));
	pt->pulse = buf[i++];

	le_assert(i == offsetof(nal_t, slope));
	pt->slope = (signed char) buf[i++]; /* make sure the sign bit extends! */

	le_assert(i == offsetof(nal_t, compass));
	pt->compass = (unsigned short) (buf[i++]  << 0);
	pt->compass |= (unsigned short) (buf[i++] << 8);

	le_assert(i == offsetof(nal_t, roll));
	pt->roll = (short) (buf[i++]  << 0);
	pt->roll |= (short) (buf[i++] << 8);

	le_assert(i == offsetof(nal_t, yaw));
	pt->yaw = (short) (buf[i++]  << 0);
	pt->yaw |= (short) (buf[i++] << 8);

	le_assert(i == offsetof(nal_t, speed));
	pt->speed = buf[i++];

	le_assert(i == offsetof(nal_t, bike));
	pt->bike = buf[i++];

	if (buf[i++] != 0) {
		warnx("%s: #%lu: trailing pad byte[%u]: 0x%X", fn, (unsigned long) rn, i, (unsigned int) buf[i-1]);
	}
	if (buf[i++] != 0) {
		warnx("%s: #%lu: trailing pad byte[%u]: 0x%X", fn, (unsigned long) rn, i, (unsigned int) buf[i-1]);
	}

	le_assert(i == sizeof(nal_t));

	return true;
}

#ifdef DO_MAIN

static void
pp_nal(nal_t *rec,			/* record data */
       size_t rn)			/* record number */
{
	static int oldlong = 0;
	static int oldlat = 0;

	printf("#%lu (nal_t): bike #%u", (unsigned long) rn, (unsigned int) rec->bike);

	if (rec->start) {
		printf(", Track Start");
		oldlong = 0;
		oldlat = 0;
	}

	if (rec->waypoint)
		printf(", Waypoint");

	if (rec->padding_flags != 0)
		printf(", [padding_flags:0x%X]", (unsigned int) rec->padding_flags);
	if (rec->padding_end != 0)
		printf(", [padding_end:0x%X]", (unsigned int) rec->padding_end);

	putchar('\n');

	printf("\tdate: %u/%02u/%02u", rec->year + 2000, rec->month, rec->day);
	printf("-%02u:%02u:%02u Z\n", rec->hour, rec->minute, rec->second);

	printf("\tpos-dec: %d.%d° %c, %d.%d° %c\n",
	       abs(rec->longitude) / 10000000,
	       abs(rec->longitude) % 10000000,
	       rec->longitude == 0 ? ' ' : rec->longitude > 0 ? 'E' : 'W',
	       abs(rec->latitude) / 10000000,
	       abs(rec->latitude) % 10000000,
	       rec->latitude == 0 ? ' ' : rec->latitude > 0 ? 'N' : 'S');

	/*
	 *  1. The absolute value of the value is the units of degrees
	 *     (i.e. given -121.135° longitude, we start with 121°).
	 *
	 *  2. Multiply the remaining decimal part by 60
	 *     (i.e. 0.135 * 60 = 8.1).
	 *
	 *  3. The whole number is the number minutes (8').
	 *
	 *  4. Take the remainder and multiply by 60.
	 *     (i.e. 0.1 * 60 = 6).
	 *
	 *  5. The resulting number becomes the seconds (6").  Seconds can
	 *     remain as a decimal number for fractions of a second.
	 *
	 *  6. Take your three sets of numbers and put them together, using the
	 *     symbols for degrees (°), minutes ('), and seconds (")
	 *     (i.e. 121° 8' 6" longitude)
	 *
	 *  7. Depending on whether the source number was a latitudinal or
	 *     longitudinal co-ordinate, and depending on the sign of the
	 *     number, add the N/S or E/W specifier.
	 *
	 * Pseudo-code (for floating-point representations):
	 *
	 *	double	decimal, minutes_tmp, seconds;
	 *	long	degrees, minutes;
	 *
	 *	degrees = lround(floor(decimal));
	 *	minutes_tmp = (decimal - floor(decimal)) * 60;
	 *	minutes = lround(floor(minutes_tmp));
	 *	seconds = (minutes_tmp - floor(minutes_tmp)) * 60;
	 */

	printf("\tpos-DMS: %d° %d' %d.%d\" %c",
	       abs(rec->longitude) / 10000000,
	       ((abs(rec->longitude) % 10000000) * 60) / 10000000,
	       ((((abs(rec->longitude) % 10000000) * 60) % 10000000) * 60) / 10000000,
	       (((((abs(rec->longitude) % 10000000) * 60) % 10000000) * 60) % 10000000) / 100,
	       rec->longitude == 0 ? ' ' : rec->longitude > 0 ? 'E' : 'W');

	printf(", %d° %d' %d.%d\" %c\n",
	       abs(rec->latitude) / 10000000,
	       ((abs(rec->latitude) % 10000000) * 60) / 10000000,
	       ((((abs(rec->latitude) % 10000000) * 60) % 10000000) * 60) / 10000000,
	       (((((abs(rec->latitude) % 10000000) * 60) % 10000000) * 60) % 10000000) / 100,
	       rec->latitude == 0 ? ' ' : rec->latitude > 0 ? 'N' : 'S');

	printf("\tcompass: %d°, roll %d°, yaw %d°",
	       (int) rec->compass, (int) rec->roll, (int) rec->yaw);
	if (rec->slope != 0)
		printf(", slope %d°", (int) rec->slope);
	putchar('\n');

	printf("\talt: %u meters\n", (unsigned int) rec->height);
	printf("\ttemp: %d °C\n", (int) rec->temperature);
	printf("\tpres: %u mb\n", (unsigned int) rec->pressure);

	if (rec->speed != 0)
		printf("\tspeed: %u km/h\n", (unsigned int) rec->speed);
	if (rec->cadence != 0)
		printf("\tcadence: %u RPM\n", (unsigned int) rec->cadence);
	if (rec->pulse != 0)
		printf("\tpulse: %u B/m\n", (unsigned int) rec->pulse);

	show_distance(oldlong, oldlat, rec->longitude, rec->latitude);
	oldlong = rec->longitude;
	oldlat = rec->latitude;

	return;
}
#endif	/* DO_MAIN */

static int
pp_gpspt(gpspt_t rec,			/* record data */
	 size_t rn)			/* record number */
{
	static int oldlong = 0;
	static int oldlat = 0;
	char dbuf[BUFSIZ];

	printf("#%lu: bike #%u", (unsigned long) rn, (unsigned int) rec.bike);

	if (rec.start) {
		printf(", Track Start");
		oldlong = 0;
		oldlat = 0;
	}

	if (rec.waypoint)
		printf(", Waypoint");

	putchar('\n');

	strftime(dbuf, (size_t) BUFSIZ, "%Y/%m/%d-%H:%M:%S Z", gmtime(&rec.date));
	printf("\tdate: %s\n", dbuf);

	strftime(dbuf, (size_t) BUFSIZ, "%Y/%m/%d-%H:%M:%S [%z]", localtime(&rec.date));
	printf("\tlocl: %s\n", dbuf);

	printf("\tpos-dec: %d.%d° %c, %d.%d° %c\n",
	       abs(rec.longitude) / 10000000,
	       abs(rec.longitude) % 10000000,
	       rec.longitude == 0 ? ' ' : rec.longitude > 0 ? 'E' : 'W',
	       abs(rec.latitude) / 10000000,
	       abs(rec.latitude) % 10000000,
	       rec.latitude == 0 ? ' ' : rec.latitude > 0 ? 'N' : 'S');

	/*
	 *  1. The absolute value of the value is the units of degrees
	 *     (i.e. given -121.135° longitude, we start with 121°).
	 *
	 *  2. Multiply the remaining decimal part by 60
	 *     (i.e. 0.135 * 60 = 8.1).
	 *
	 *  3. The whole number is the number minutes (8').
	 *
	 *  4. Take the remainder and multiply by 60.
	 *     (i.e. 0.1 * 60 = 6).
	 *
	 *  5. The resulting number becomes the seconds (6").  Seconds can
	 *     remain as a decimal number for fractions of a second.
	 *
	 *  6. Take your three sets of numbers and put them together, using the
	 *     symbols for degrees (°), minutes ('), and seconds (")
	 *     (i.e. 121° 8' 6" longitude)
	 *
	 *  7. Depending on whether the source number was a latitudinal or
	 *     longitudinal co-ordinate, and depending on the sign of the
	 *     number, add the N/S or E/W specifier.
	 *
	 * Pseudo-code (for floating-point representations):
	 *
	 *	double	decimal, minutes_tmp, seconds;
	 *	long	degrees, minutes;
	 *
	 *	degrees = lround(floor(decimal));
	 *	minutes_tmp = (decimal - floor(decimal)) * 60;
	 *	minutes = lround(floor(minutes_tmp));
	 *	seconds = (minutes_tmp - floor(minutes_tmp)) * 60;
	 */

	printf("\tpos-DMS: %d° %d' %d.%d\" %c",
	       abs(rec.longitude) / 10000000,
	       ((abs(rec.longitude) % 10000000) * 60) / 10000000,
	       ((((abs(rec.longitude) % 10000000) * 60) % 10000000) * 60) / 10000000,
	       (((((abs(rec.longitude) % 10000000) * 60) % 10000000) * 60) % 10000000) / 100,
	       rec.longitude == 0 ? ' ' : rec.longitude > 0 ? 'E' : 'W');

	printf(", %d° %d' %d.%d\" %c\n",
	       abs(rec.latitude) / 10000000,
	       ((abs(rec.latitude) % 10000000) * 60) / 10000000,
	       ((((abs(rec.latitude) % 10000000) * 60) % 10000000) * 60) / 10000000,
	       (((((abs(rec.latitude) % 10000000) * 60) % 10000000) * 60) % 10000000) / 100,
	       rec.latitude == 0 ? ' ' : rec.latitude > 0 ? 'N' : 'S');

	printf("\tcompass: %d°, roll %d°, yaw %d°",
	       (int) rec.compass, (int) rec.roll, (int) rec.yaw);
	if (rec.slope != 0)
		printf(", slope %d°", (int) rec.slope);
	putchar('\n');

	printf("\talt: %u meters\n", (unsigned int) rec.height);
	printf("\ttemp: %d °C\n", (int) rec.temperature);
	printf("\tpres: %u mb\n", (unsigned int) rec.pressure);

	if (rec.speed != 0)
		printf("\tspeed: %u km/h\n", (unsigned int) rec.speed);
	if (rec.cadence != 0)
		printf("\tcadence: %u RPM\n", (unsigned int) rec.cadence);
	if (rec.pulse != 0)
		printf("\tpulse: %u B/m\n", (unsigned int) rec.pulse);

	show_distance(oldlong, oldlat, rec.longitude, rec.latitude);
	oldlong = rec.longitude;
	oldlat = rec.latitude;

	return (0);
}

/*
 * Autor: Dennis Heynlein (c-logic)
 * Datum: 12.06.2012 21:35
 * C-Implementation for Distance & Bearing from
 * http://www.movable-type.co.uk/scripts/latlong.html
 *
 * XXX calculate time delta and average speed too!
 */
static void
show_distance(int oldlong,
	      int oldlat,
	      int longitude,
	      int latitude)
{
	double dist;
	double brng;

	if (oldlong || oldlat) {
		CalcDist2(oldlong, oldlat, longitude, latitude, &dist, &brng);
		printf("\tdistance: %0.2lf m bearing: %0.1lf°\n", dist, brng);
	}

	return;
}

static void
CalcDist2(int long1,
	  int lati1,
	  int long2,
	  int lati2,
	  double *dist,
	  double *brng)
{
	const double R = 6371000;

	double lat1=lati1/1E7*M_PI/180.;
	double lat2=lati2/1E7*M_PI/180.;
	double lon1=long1/1E7*M_PI/180.;
	double lon2=long2/1E7*M_PI/180.;
	double dLat = (lat2-lat1);
	double dLon = (lon2-lon1);
	double dPhi = log(tan(lat2/2.+M_PI/4.)/tan(lat1/2.+M_PI/4.));
	double q = (dPhi != 0.) ? dLat/dPhi : cos(lat1);  /* E-W line givesdPhi=0 */

	/* if dLon over 180° take shorter rhumb across 180° meridian: */
	if (fabs(dLon) > M_PI) {
		dLon = (dLon > 0.) ? -(2.*M_PI-dLon) : (2.*M_PI+dLon);
	}
	*dist = sqrt(dLat*dLat + q*q*dLon*dLon) * R ;
	*brng = atan2(dLon, dPhi)*180./M_PI;
}


#ifndef DO_MAIN
static gbfile* fin;

static void
wsg2000_rd_init(const char *fname)
{
	fin = gbfopen(fname, "r", MYNAME);
}

static void
wsg2000_rd_deinit(void)
{
	gbfclose(fin);
}

static void
wsg2000_read(void)
{
	char buf[NAL_BLOCK_SZ];
	gpspt_t pt;
	size_t nb = 0;
	size_t nr = 0;
	route_head *trk = route_head_alloc();

	track_add_head(trk);

	while ((nb = gbfread(buf, (size_t) 1, sizeof(buf), fin)) == sizeof(buf)) {
		waypoint *wpt;

		if (!fill_gpspt(&pt, (unsigned char *) buf, fin->name, nr)) {
			warnx("%s: record #%lu: failed to convert properly",
			      fin->name, (unsigned long) nr);
		}
		if (! is_nal_p(pt, fin->name, nr)) {
			continue;
		}
		if (global_opts.debug_level > 1)  {
			(void) pp_gpspt(pt, nr);
		}
		wpt = waypt_new();

                wpt->creation_time = pt.date;

		wpt->latitude = pt.latitude / 1.0e7;
		wpt->longitude = pt.longitude / 1.0e7;
		wpt->altitude = pt.height * 1.0;

		wpt->wpt_flags.speed = 1;
		wpt->speed = pt.speed * 1.0e3 / 60.0; /* m/s, from km/h */

		wpt->wpt_flags.temperature = 1;
		wpt->temperature = pt.temperature * 1.0;

		wpt->cadence = pt.cadence;
		wpt->heartrate = pt.pulse;

		wpt->wpt_flags.course = 1;
		/*
		 * XXX this may be wrong, since this is the magnetic compass
		 * reading, which has nothing to do with the track course
		 * direction.  Maybe it should be the bearing calculated by
		 * CalcDist2(), in which case maybe it should just be left out.
		 */
		wpt->course = pt.compass * 1.0;

		if (pt.start) {
			char rnm[sizeof("TRACK@YYYY-MM-DDTHH:MM:SSZ")];
			struct tm *rt;

			rt = gmtime(&pt.date);
			strftime(rnm, sizeof(rnm), "TRACK@%FT%TZ", rt);
			trk->rte_name = xstrdup(rnm);
		}
		track_add_wpt(trk, wpt);

                if (pt.waypoint) {
			char rnm[sizeof("WAYPT@YYYY-MM-DDTHH:MM:SSZ")];
			struct tm *rt;
			waypoint *temp = waypt_dupe(wpt);

			/*
			 * XXX for some extremly undocumented and very hidden
			 * reason the hyphens get stripped from the "shortname"
			 * when it gets output in a GPX file in a <name>
			 * attribute.
			 *
			 * There is a rumour in online forums that Garmin's
			 * waypoint format does not allow a hyphen in a
			 * waypoint name, but why GPSBabel insists on extending
			 * this limitation to GPX format is beyond me!  I can't
			 * find anything in the GPX documentation that even
			 * suggests there's any limitation to the characters
			 * allowed in a name.
			 *
			 * I can't even find in the code where hyphens are
			 * stripped from anything.
			 */

			rt = gmtime(&pt.date);
			strftime(rnm, sizeof(rnm), "WAYPT@%FT%TZ", rt);
			temp->shortname = xstrdup(rnm);
			waypt_add(temp);
                }

		nr++;
	}
	if (nb > 0) {
		warnx("%s: record #%lu: read incomplete record of %lu bytes",
		      fin->name, (unsigned long) nr, (unsigned long) nb);
		return;
	}
	if (!gbfeof(fin)) {
		warn("%s: finished reading before EOF (last record #%lu was %lu bytes short)",
		     fn, (unsigned long) nr, (unsigned long) (sizeof(buf) - nb));
	}
	if (gbferror(fin)) {
		warn("%s: error reading after %lu records",
		     fn, (unsigned long) nr);
		return;
	}
}

static
arglist_t wsg2000_args[] = {
	ARG_TERMINATOR
};

ff_vecs_t wsg2000_vecs = {
	ff_type_file,
	{
		ff_cap_read 			/* waypoints */,
	  	ff_cap_read 			/* tracks */,
	  	ff_cap_none 			/* routes */
	},
	wsg2000_rd_init,
	NULL,
	wsg2000_rd_deinit,
	NULL,
	wsg2000_read,
	NULL,
	NULL,
	wsg2000_args,
	CET_CHARSET_ASCII, 0			/* ascii is the expected character set */
						/* not fixed, can be changed through command line parameter */
};
#endif

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " CPPFLAGS='-DDO_MAIN' LDLIBS=-lm " fn " && echo 'run with one or more WSG-2000 .NAL files'"))
 * End:
 */
