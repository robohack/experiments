/*
 * trying to avoid requiring external tests for <sys/cdefs.h>, etc.
 */
#if (!defined(HAVE_SYS_CDEFS_H) &&                                      \
     (defined(__linux__) ||						\
      defined(BSD) ||                                                   \
      defined(__NetBSD__) ||                                            \
      defined(__FreeBSD__) ||                                           \
      defined(__OpenBSD__) ||                                           \
      defined(__Darwin__) ||                                            \
      defined(__DragonFly__) ||                                         \
      defined(__APPLE__)))
# define HAVE_SYS_CDEFS_H
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
# define HAVE_SYS_PARAM_H
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
# define HAVE_STDDEF_H
# define HAVE_STDINT_H
# define HAVE_SYS_TYPES_H		/* ??? */
# define HAVE_WCHAR_H			/* ??? */
#endif

#ifdef HAVE_STDBOOL_H
# include <stdbool.h>
#else /* !HAVE_STDBOOL_H  */

/*
 * C99 _Bool is a very special type as it may only ever have the values of
 * 'true'(1) or 'false'(0), but if you don't have it then you _must_ use 'int'.
 *
 * Note the standard says:
 *
 *     6.2.5
 *
 *     An object declared as type _Bool is large enough to store the values 0
 *     and 1.
 *
 *     The size cannot be smaller than one byte.  But it would be legal to be
 *     larger than one byte.
 *
 * If you try to use "unsigned char" for "bool", and you happen to assign
 * exactly 256 to a variable of that type (at least on a little endian
 * machine?), then it will evaluate as false!
 *
 * N.B.:  Using 'unsigned int' may also run afoul of standard conversions when
 * the unary negation operator is used in a parameter expression since the
 * result of '!' is of course just 'int'
 *
 * n.b.:  setting true to the !false value covers the case of one writing
 * effectively "if (! false) blah..."
 *
 *     §3.3.3.3: "The result of the logical negation operator ! is 0 if the
 *     value of its operand compares unequal to 0, 1 if the value of its operand
 *     compares equal to 0."
 */
typedef enum bool_e { false = 0U, true = !false } bool;

#endif /* !HAVE_STDBOOL_H  */

#ifdef HAVE_STDDEF_H
# include <stddef.h>
#endif
#ifdef HAVE_STDINT_H
# include <stdint.h>
#endif
#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif
#ifdef HAVE_WCHAR_H
# include <wchar.h>
#endif

#include <limits.h>			/* needed for CHAR_BIT */
#include <stdio.h>			/* nearly forever! */
#include <stdlib.h>			/* C89 */

#ifndef TICK_T_DEFINED			/* xxx hmmmm... is it ever, anywhere? */
# define TICK_T_DEFINED
typedef uint64_t tick_t;
#endif

/*
 * using "bool" as a type for a function parameter is a losing game with some
 * compilers, on some systems, because it will be a type with sizeof(bool)==1,
 * so it's the same problem as using a "char" or "short" as the type of a
 * function parameter.
 *
 * The "usual conversions" in C89/C90 require that any expression given as an
 * argument when calling such a function without a prototype in scope be
 * widened to "int".
 *
 * However a function defined with a prototype in scope may be compiled such
 * that "int"'s worth of argument to be passed to it.
 *
 * Since prototypes are not required it is incumbent on implementers to ensure
 * that they define their functions in such a way that they will still operate
 * properly even if the "usual conversions" have been applied to arguments.
 * One way to do that would be to continue to use old-style function
 * definitions, but too many people seem to frown on that these days.
 *
 * It's an impossible situation.  Don't _ever_ use these as parameter types!
 */
bool fubar_bool(int, bool, int, unsigned int);

bool
fubar_bool(int c,
	   bool fb,
	   int i,
	   unsigned int ui)
{
	if (fb != (bool) i) {
		printf("fubar_bool: %d, int conversion failed for %d != %d\n", c, (int) fb, i);
	}
	if (fb != (bool) ui) {
		printf("fubar_bool: %d, unsigned int conversion failed for %u != %u\n", c, (unsigned int) fb, ui);
	}
	return fb;
}

char fubar_char(int, char, int, unsigned int);

char
fubar_char(int c,
	   char fc,
	   int i,
	   unsigned int ui)
{
	if (fc != i) {
		printf("fubar_char: %d, int conversion failed for %d != %d\n", c, (int) fc, i);
	}
	if (fc != ui) {
		printf("fubar_char: %d, unsigned int conversion failed for %u != %u\n", c, (unsigned int) fc, ui);
	}
	return fc;
}

unsigned char fubar_uchar(int, unsigned char, int, unsigned int);

unsigned char
fubar_uchar(int c,
	    unsigned char fc,
	    int i,
	    unsigned int ui)
{
	if (fc != i) {
		printf("fubar_uchar: %d, int conversion failed for %d != %d\n", c, (int) fc, i);
	}
	if (fc != ui) {
		printf("fubar_uchar: %d, unsigned int conversion failed for %u != %u\n", c, (unsigned int) fc, ui);
	}
	return fc;
}

short fubar_short(int, short, int, unsigned int);

short
fubar_short(int c,
	    short fc,
	    int i,
	    unsigned int ui)
{
	if (fc != i) {
		printf("fubar_short: %d, int conversion failed for %d != %d\n", c, (int) fc, i);
	}
	if (fc != ui) {
		printf("fubar_short: %d, unsigned int conversion failed for %u != %u\n", c, (unsigned int) fc, ui);
	}
	return fc;
}

unsigned short fubar_ushort(int, unsigned short, int, unsigned int);

unsigned short
fubar_ushort(int c,
	     unsigned short fc,
	     int i,
	     unsigned int ui)
{
	if (fc != i) {
		printf("fubar_ushort: %d, int conversion failed for %d != %d\n", c, (int) fc, i);
	}
	if (fc != ui) {
		printf("fubar_ushort: %d, unsigned int conversion failed for %u != %u\n", c, (unsigned int) fc, ui);
	}
	return fc;
}

/*
 * if you provide a prototype matching what you think you're defining:
 *
 *	char fubar_char_noproto(char);
 *
 * then Clang says (which is entirely true and to be expected!):
 *
 * warning: promoted type 'int' of K&R function parameter is not compatible with the parameter type 'char' declared in a previous prototype [-Wknr-promoted-parameter]
 *
 * and without the prototype Clang says, of course:
 *
 * warning: no previous prototype for function 'fubar_char_noproto' [-Wmissing-prototypes]
 */
char
fubar_char_noproto(fc)			/* warning: function declaration isn't a prototype */
	char fc;			/* N.B.:  hidden de-promotion!!! */
{
	return fc;
}

short
fubar_short_noproto(fc)			/* warning: function declaration isn't a prototype */
	short fc;			/* N.B.:  hidden de-promotion!!! */
{
	return fc;
}

/* N.B.:  sizeof(char) == 1 */

struct struct_a
{
	char        c;
	/* padding: sizeof(short int) - sizeof(char) */
	short int   s;
};

struct struct_b
{
	short int   s;
	char        c;
	/* padding: sizeof(int) - (sizeof(short int) + sizeof(char)) */
	int         i;
};

struct struct_c
{
	char        c;
	/* padding: sizeof(double) - sizeof(char) */
	double      d;
	int         s;
	/* alignment padding: sizeof(double) - sizeof(int) */
};

struct struct_d
{
	double      d;
	int         s;
	char        c;
	/* alignment padding: sizeof(double) - (sizeof(int) + sizeof(char)) */
};

#include <poll.h>

#define alignof(type)		((int) (sizeof(struct {char d; type x;}) - sizeof(type)))

static void
structs(void)
{
	printf("sizeof(struct struct_a) = %d, alignment: %d\n",
	       (int) sizeof(struct struct_a), alignof(struct struct_a));
	printf("sizeof(struct struct_b) = %d, alignment: %d\n",
	       (int) sizeof(struct struct_b), alignof(struct struct_b));
	printf("sizeof(struct struct_c) = %d, alignment: %d\n",
	       (int) sizeof(struct struct_c), alignof(struct struct_c));
	printf("sizeof(struct struct_d) = %d, alignment: %d\n",
	       (int) sizeof(struct struct_d), alignof(struct struct_d));
	printf("sizeof(struct pollfd) = %d, alignment: %d\n",
	       (int) sizeof(struct pollfd), alignof(struct pollfd));

	return;
}

#include <arpa/inet.h>

int main(void);

int
main()
{
	volatile unsigned int bits;
	volatile unsigned int char_bits;
	bool	vb = true;
	bool	rb;

	unsigned char	uc;
	signed char	sc;
	char	vc = L'1';
	char	rc;
	unsigned char	ruc;

	short	vs = 1;
	short	rs;
	unsigned short	vus = 1;
	unsigned short	rus;

	int	vi = 1;
	int	ri;

	ri = vb;
	ri = (bool) vb;
	ri = true;

	ri = vc;
	ri = vs;
	ri = vi;

	(void) ri;	/* avoid "warning: variable 'ri' set but not used" */
	(void) rus;
	(void) ruc;

	char_bits = 0;
	bits = 1;
	while ((~0U & (~0U >> (bits - 1))) != 1) {
		bits++;
	}
	if (bits % sizeof(unsigned int) == 0) {
		char_bits = bits / sizeof(unsigned int);
	}
	printf("char bits = %u %s [CHAR_BIT == %d].\n", char_bits,
	       char_bits == CHAR_BIT ? "==" : "!=", (int) CHAR_BIT);

#ifdef WEOF
	/* are wide chars signed or unsigned? */
	printf("~(wchar_t) L'\\0' = 0x%x\n", ~((wchar_t) L'\0'));
	printf("(unsigned long) ~(wchar_t) L'\\0' = 0x%lx\n", (unsigned long) (~((wchar_t) L'\0')));
#endif

	uc = 256;			/* warning: large integer implicitly truncated to unsigned type */
	/* warning: unsigned conversion from 'int' to 'unsigned char' changes value from '256' to '0' [-Woverflow] */
	if (uc) {
		printf("Oh OH!  uc=256 is true!!!\n");
	} else {
		printf("uc=256 is %u!!!\n", (unsigned int) uc);
	}
	sc = 256;			/* warning: overflow in implicit constant conversion */
	/* warning: overflow in conversion from 'int' to 'signed char' changes value from '256' to '0' [-Woverflow] */
	if (sc) {
		printf("Oh OH!  sc=256 is true!!!\n");
	} else {
		printf("sc=256 is %u!!!\n", (unsigned int) sc);
	}
	sc = 257;			/* warning: overflow in implicit constant conversion */
	/* warning: overflow in conversion from 'int' to 'signed char' changes value from '257' to '1' [-Woverflow] */
	if (!sc) {
		printf("Oh OH!  sc=257 is NOT true!!!\n");
	} else {
		printf("sc=257 is %u!!!\n", (unsigned int) sc);
	}

	putchar('\n');

	printf("sizeof 'x' = %u.\n", (unsigned int) sizeof('x'));
#if (__STDC__ - 0) != 0
	printf("sizeof L'x' = %u.\n", (unsigned int) sizeof(L'x'));
#elif defined(__STDC_VERSION__) (__STDC_VERSION__ - 0) >= 2011121L
	printf("sizeof u'x' = %u.\n", (unsigned int) sizeof(u'x'));
	printf("sizeof U'x' = %u.\n", (unsigned int) sizeof(U'x'));
#endif
	printf("sizeof bool = %u.\n", (unsigned int) sizeof(bool));
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ - 0) >= 199901L
	printf("sizeof _Bool = %u.\n", (unsigned int) sizeof(_Bool));
#else
	/* both GCC and Clang offer _Bool even with -std=c89 (but newer versions offer a warning with -pedantic) */
	printf("sizeof _Bool = %u.\n", (unsigned int) sizeof(_Bool));
#endif
	printf("true = %ju, false = %ju, !false = %ju\n", (uintmax_t) true, (uintmax_t) false, (uintmax_t) (! false));
	printf("sizeof char = %u.\n", (unsigned int) sizeof(char));
	printf("sizeof unsigned char = %u.\n", (unsigned int) sizeof(unsigned char));
	printf("sizeof signed char = %u.\n", (unsigned int) sizeof(signed char));
#ifdef WEOF
	printf("sizeof wchar_t = %u.\n", (unsigned int) sizeof(wchar_t));
	/* xxx strictly only ISO C90, not in C89 */
	printf("sizeof wint_t = %u.\n", (unsigned int) sizeof(wint_t));
#endif
	printf("sizeof 1 = %u.\n", (unsigned int) sizeof(1));
	printf("sizeof short = %u.\n", (unsigned int) sizeof(short));
	printf("sizeof int = %u.\n", (unsigned int) sizeof(int));
	printf("sizeof 1L = %u.\n", (unsigned int) sizeof(1L));
	printf("sizeof long = %u.\n", (unsigned int) sizeof(long));
	printf("sizeof 1LL = %u.\n", (unsigned int) sizeof(1LL));
	printf("sizeof long long = %u.\n", (unsigned int) sizeof(long long));
#if defined(__NetBSD__)
	printf("sizeof longlong_t = %u.\n", (unsigned int) sizeof(longlong_t));
#endif
	printf("sizeof intmax_t = %u.\n", (unsigned int) sizeof(intmax_t));

	printf("sizeof uint8_t = %u.\n", (unsigned int) sizeof(uint8_t));
	printf("sizeof uint16_t = %u.\n", (unsigned int) sizeof(uint16_t));
	printf("sizeof uint32_t = %u.\n", (unsigned int) sizeof(uint32_t));
	printf("sizeof uint64_t = %u.\n", (unsigned int) sizeof(uint64_t));

#if !defined(__linux__) || !defined(__STRICT_ANSI__)
	/* quad_t is not defined with "gcc -std=c89", etc. */
	printf("sizeof quad_t = %u.\n", (unsigned int) sizeof(quad_t));
#endif
	printf("sizeof intptr_t = %u.\n", (unsigned int) sizeof(intptr_t));
	printf("sizeof uintptr_t = %u.\n", (unsigned int) sizeof(uintptr_t));
	printf("sizeof ptrdiff_t = %u.\n", (unsigned int) sizeof(ptrdiff_t));
	printf("sizeof size_t = %u.\n", (unsigned int) sizeof(size_t));
	printf("sizeof ssize_t = %u.\n", (unsigned int) sizeof(ssize_t));
	printf("sizeof off_t = %u.\n", (unsigned int) sizeof(off_t));
	printf("sizeof fpos_t = %u.\n", (unsigned int) sizeof(fpos_t));
	printf("sizeof time_t = %u.\n", (unsigned int) sizeof(time_t));
	printf("sizeof clock_t = %u.\n", (unsigned int) sizeof(clock_t));
	printf("sizeof tick_t = %u.\n", (unsigned int) sizeof(tick_t));
	printf("sizeof char * = %u.\n", (unsigned int) sizeof(char *));
	printf("sizeof void * = %u.\n", (unsigned int) sizeof(void *));
	printf("sizeof in_addr_t = %u.\n", (unsigned int) sizeof(in_addr_t));
	printf("sizeof in_port_t = %u.\n", (unsigned int) sizeof(in_port_t));
#if !defined(__linux__) || !defined(__STRICT_ANSI__)
	printf("sizeof u_short = %u.\n", (unsigned int) sizeof(u_short));
#endif
	printf("sizeof double = %u.\n", (unsigned int) sizeof(double));
	printf("sizeof float = %u.\n", (unsigned int) sizeof(float));

	/*
	 * In "modern" standard C, i.e. C89 and newer since prototypes became
	 * an _optional_ feature, function arguments are converted, as if by
	 * assignment, to the declared types of the formal parameters.
	 *
	 * However in all the following cases GCC (prior to 4.6?), at least,
	 * complains.
	 *
	 * I think the issue in older GCC is that an expression result as an
	 * argument in a function call _always_ gets treated with the "usual
	 * integer conversions" before being used as an argument
	 */

	rb = fubar_bool(1, vb, vb, vb);
	rb = fubar_bool(2, (bool) vb, (bool) vb, (bool) vb);
	rb = fubar_bool(3, true, true, true);
	rb = fubar_bool(4, (bool) true, (bool) true, (bool) true);
	rb = fubar_bool(5, -1, -1, -1);
	rb = fubar_bool(6, '0', '0', '0');
	rb = fubar_bool(7, L'0', L'0', L'0');
	rb = fubar_bool(8, (bool) '0', (bool) '0', (bool) '0');
	rb = fubar_bool(9, '\0', '\0', '\0');
	rb = fubar_bool(10, -99, -99, -99);

	rc = fubar_char(1, vc, vc, vc);
	rc = fubar_char(2, true, true, true);
	rc = fubar_char(3, (char) true, (char) true, (char) true);
	rc = fubar_char(4, EOF, EOF, EOF);
	rc = fubar_char(5, '0', '0', '0');
	rc = fubar_char(6, L'0', L'0', L'0');
	rc = fubar_char(7, (char) '0', (char) '0', (char) '0');

	ruc = fubar_uchar(1, (unsigned char) vc, (unsigned char) vc, (unsigned char) vc);
	ruc = fubar_uchar(2, true, true, true);
	ruc = fubar_uchar(3, (unsigned char) true, (unsigned char) true, (unsigned char) true);
	ruc = fubar_uchar(4, EOF, EOF, EOF); /* xxx passing EOF to uchar truncates it to 255! */
	ruc = fubar_uchar(5, '0', '0', '0');
	ruc = fubar_uchar(6, L'0', L'0', L'0');
	ruc = fubar_uchar(7, (unsigned char) '0', (unsigned char) '0', (unsigned char) '0');

	rs = fubar_short(1, vs, vs, vs);
	rs = fubar_short(2, true, true, true);
	rs = fubar_short(3, (short) true, (short) true, (short) true);
	rs = fubar_short(4, '0', '0', '0');
	rs = fubar_short(5, L'0', L'0', L'0');
	rs = fubar_short(6, (short) '0', (short) '0', (short) '0');

	rus = fubar_ushort(1, vus, vus, vus);
	rus = fubar_ushort(2, (unsigned short) vs, (unsigned short) vs, (unsigned short) vs);
	rus = fubar_ushort(3, true, true, true);
	rus = fubar_ushort(4, (unsigned short) true, (unsigned short) true, (unsigned short) true);
	rus = fubar_ushort(5, -1, -1, -1); /* xxx passing -1 to ushort truncates it to 65536! */
	rus = fubar_ushort(6, '0', '0', '0');
	rus = fubar_ushort(7, L'0', L'0', L'0');
	rus = fubar_ushort(8, (unsigned short) '0', (unsigned short) '0', (unsigned short) '0');

	/* no warnings for these!  Yay! */
	rc = fubar_char_noproto(vc);
	rc = fubar_char_noproto(true);
	rc = fubar_char_noproto((char) true);
	rc = fubar_char_noproto('0');
	rc = fubar_char_noproto(L'0');
	rc = fubar_char_noproto((char) '0');

	/* no warnings for these!  Yay! */
	rs = fubar_short_noproto(vs);
	rs = fubar_short_noproto(1);
	rs = fubar_short_noproto((short) 1);

	/*
	 * NOTE: the "sizeof" unary operator, when applied to an expression
	 * yeilds the same result as if "sizeof(type)", where "type" is the
	 * natural type of the expression result.  The "sizeof" operator does
	 * not itself cause any of the usual conversion to be applied to the
	 * expression when determining its type (provided the expression does
	 * not itself include operators which _do_ perform the usual
	 * conversions).
	 *
	 * Because of the lack of the usual conversions, sizeof() applied to a
	 * plain variable name is the size of the (visible) storage allocated
	 * for it (it may have invisible padding for alignment, but that is
	 * never accounted for).  However the size of an expression is the size
	 * of the storage required for the natural (un-cast) result of that
	 * expression, and all the usual conversions are applied in the
	 * expression.
	 *
	 * Also note:  "sizeof(expr)" is the same as "sizeof (expr)", i.e. if
	 * the thing in the parenthesis is not a _type_ name, then "sizeof" is
	 * by definition acting as a unary operator.  The space is as
	 * irrelevant as it is for function calls or the value given to the
	 * "return" statement.
	 *
	 * WARNING:  "sizeof expr" (where 'expr' is one term, i.e. a variable
	 * name) is _NOT_ always the same as "sizeof (expr)" where parens are
	 * required to encapsulate the result of a multi-term expression, _NOR_
	 * is it the same when 'expr' is a constant value (which is also subject
	 * to the usual conversions)!
	 */
	printf("\n");
	printf("sizeof(rb) == %d (1)\n", (int) sizeof(rb));
	printf("sizeof rb == %d (1)\n", (int) sizeof rb);
	printf("sizeof (rb + rb) == %d (4!!!)\n", (int) sizeof (rb + rb));
	printf("sizeof(true) == %d (4!!!)\n", (int) sizeof(true));
	printf("sizeof true == %d (4!!!)\n", (int) sizeof(true));

	printf("\n");
	printf("sizeof(rc) == %d (1)\n", (int) sizeof(rc));
	printf("sizeof rc == %d (1)\n", (int) sizeof rc);
	printf("sizeof (rc + rc) == %d (4!!!)\n", (int) sizeof (rc + rc));
	printf("sizeof('c') == %d (4!!!)\n", (int) sizeof('c'));
	printf("sizeof 'c' == %d (4!!!)\n", (int) sizeof('c'));

	printf("\n");
	printf("sizeof(rs) == %d (2)\n", (int) sizeof(rs));
	printf("sizeof rs == %d (2)\n", (int) sizeof rs);
	printf("sizeof (rs + rs) == %d (4!!!)\n", (int) sizeof (rs + rs));
	/* n.b.:  there is no syntax to represent a short constant */
	printf("sizeof(0xFF) == %d (4!)\n", (int) sizeof(0xFF));
	printf("sizeof 0xFF == %d (4!)\n", (int) sizeof 0xFF);
	printf("sizeof(0xFFFF) == %d (4!)\n", (int) sizeof(0xFFFF));
	printf("sizeof 0xFFFF == %d (4!)\n", (int) sizeof 0xFFFF);
	printf("sizeof(0xFFFFFF) == %d (4!)\n", (int) sizeof(0xFFFFFF));
	printf("sizeof 0xFFFFFF == %d (4!)\n", (int) sizeof 0xFFFFFF);

	printf("\n");
	/*
	 * sizeof cannot show you the size of the natural result of an
	 * expression passed as an argument, even when there's a cast on the
	 * result to narrow it back down to the desired size:
	 *
	 * the "use" of the result as an argument is like applying a unary
	 * operator again, so the the usual unary conversions apply again
	 */

	/*
	 * here sizeof() is telling us the natural result size, not the size
	 * that would be pushed onto the stack if it were an argument.
	 */
	printf("sizeof((char) (rc + rc)) == %d (1)\n",
	       (int) sizeof((char) (rc + rc))); /* too smart! */
	/*
	 * here we simulate the "use" caused by being an argument (by adding a
	 * zero cast to the same type) so that sizeof() will tell us the size
	 * that would be pushed onto the stack if it were an argument.
	 */
	printf("sizeof((char) (rc + rc) + (char) 0) == %d (4)\n",
	       (int) sizeof((char) (rc + rc) + (char) 0)); /* got you! */
	/* adding a char constant is the same.... */
	printf("sizeof(rc + rc + L'\\0') == %d (4)\n",
	       (int) sizeof(rc + rc + L'\0'));

	/* this may not do what you think it does because '\0' is an int */
	printf("sizeof(rc + rc + '\\0') == %d (4)\n",
	       (int) sizeof(rc + rc + '\0'));

	printf("sizeof(rc + rc + L'\\0') == %d (4)\n",
	       (int) sizeof(rc + rc + L'\0'));

	/* */ {
		const char *a_ptr_to_str = "some str";
		char an_arr_of_ch[] = "another str";

		printf("sizeof(a_ptr_to_str) = %zd\n", /* warning: ISO C90 does not support the 'z' gnu_printf length modifier [-Wformat=] */
		       sizeof(a_ptr_to_str));
		printf("sizeof(an_arr_of_ch) = %zd\n", /* warning: ISO C90 does not support the 'z' gnu_printf length modifier [-Wformat=] */
		       sizeof(an_arr_of_ch));
	}

	structs();

	printf("sizeof \"\" = %u.\n", (unsigned int) sizeof(""));

	exit(0);
}


/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " CSTD='c89' " fn " && ./" fn))
 * End:
 */
