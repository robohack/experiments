/* -*- mode: c; coding: iso-8859-1; -*- */
#include <ctype.h>
#include <limits.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

/*
 * <ctype.h> usually just defines this as UCHAR_MASK and assumes that the
 * result with match CHAR_BIT, but to be pedantic we will use the most portable
 * definition:
 */
#undef _CTYPE_MASK
#define _CTYPE_MASK		((unsigned char) ~(~0UL << CHAR_BIT))

#define _CTYPE_NUM_CHARS	(1U << CHAR_BIT)

/* XXX these are the same macros as NetBSD's, and so must be identical */
#if defined(_U) && _U != 0x01
# error "_U != 0x01"
#elif !defined(_U)
# define _U	0x01
#endif
#if defined(_L) && _L != 0x02
# error "_L != 0x02"
#elif !defined(_L)
# define _L	0x02
#endif
#if defined(_N) && _N != 0x04
# error "_N != 0x04"
#elif !defined(_N)
# define _N	0x04
#endif
#if defined(_S) && _S != 0x08
# error "_S != 0x08"
#elif !defined(_S)
# define _S	0x08
#endif
#if defined(_P) && _P != 0x10
# error "_P != 0x10"
#elif !defined(_P)
# define _P	0x10
#endif
#if defined(_C) && _C != 0x20
# error "_C != 0x20"
#elif !defined(_C)
# define _C	0x20
#endif
#if defined(_X) && _X != 0x40
# error "_X != 0x40"
#elif !defined(_X)
# define _X	0x40
#endif
#if defined(_B) && _B != 0x80
# error "_B != 0x80"
#elif !defined(_B)
# define _B	0x80
#endif

/*
 * XXX these require a GCC-like "statement expressions" feature, i.e. allowing a
 * a block ("{}") inside an expression so as to be able to declare variables
 * local to the expression
 */
#define MY__IS_CTYPER(C, MSK)						\
	({								\
		const unsigned int _ctype_ui_ = (unsigned int) (C);	\
		const unsigned char _ctype_uc_ = (unsigned char) _ctype_ui_; \
		(sizeof(C) > 1 && _ctype_ui_ == (unsigned int) -1) ? 0 : \
			((int) ((my_ctype + 1)[_ctype_uc_] & MSK));	\
	})

#define	MY_ISDIGIT(c)	MY__IS_CTYPER(c, _N)
#define	MY_ISLOWER(c)	MY__IS_CTYPER(c, _L)
#define	MY_ISSPACE(c)	MY__IS_CTYPER(c, _S)
#define	MY_ISPUNCT(c)	MY__IS_CTYPER(c, _P)
#define	MY_ISUPPER(c)	MY__IS_CTYPER(c, _U)
#define	MY_ISALPHA(c)	MY__IS_CTYPER(c, (_U|_L))
#define	MY_ISXDIGIT(c)	MY__IS_CTYPER(c, (_N|_X))
#define	MY_ISALNUM(c)	MY__IS_CTYPER(c, (_U|_L|_N))
#define	MY_ISPRINT(c)	MY__IS_CTYPER(c, (_P|_U|_L|_N|_B))
#define	MY_ISGRAPH(c)	MY__IS_CTYPER(c, (_P|_U|_L|_N))
#define	MY_ISCNTRL(c)	MY__IS_CTYPER(c, _C)

#define	MY_TOLOWER(C)							\
	({								\
		const unsigned int _ctype_ui_ = (unsigned int) (C);	\
		const unsigned char _ctype_uc_ = (unsigned char) _ctype_ui_; \
		(sizeof(C) > 1 && _ctype_ui_ == (unsigned int) -1) ? -1 :\
			((int) ((my_tolower_tab + 1)[_ctype_uc_]));	\
	})

#if 0	/* xxx untested -- though it is just like MY_TOLOWER() */
#define	MY_TOUPPER(C)							\
	({								\
		const unsigned int _ctype_ui_ = (unsigned int) (C);	\
		const unsigned char _ctype_uc_ = (unsigned char) _ctype_ui_; \
		(sizeof(C) > 1 && _ctype_ui_ == (unsigned int) -1) ? -1 :\
			((int) ((my_toupper_tab + 1)[_ctype_uc_])); 	\
	})
#endif

/*
 * some crazy ASCII-only range-checking implementation (that mostly works)
 *
 * (I don't remember where I found this.....)
 *
 * XXX XXX XXX except not until I fixed the use of shadowed local identifiers!!!
 *
 * XXX requires GCC-like __typeof__(), as well as the GCC-like feature of
 * putting a block ("{}") inside an expression (so-called "statement
 * expressions") so as to be able to declare variables local to the expression
 *
 * The big advantage is the lack of tables.
 *
 * The big disadvantage is that it's impossible to switch at run-time to a
 * different locale, even within 8-bit locales.
 */
#define _IN_RANGE(X, A, B)						\
    ({									\
      const __typeof__(X) _r = (X);					\
      _r >= (A) && _r <= (B);						\
    })

#define na_isascii(X) (_IN_RANGE((X), 0, 0x7F))
#define na_isdigit(X) (_IN_RANGE((X), '0', '9'))
#define na_isupper(X) (_IN_RANGE((X), 'A', 'Z'))
#define na_islower(X) (_IN_RANGE((X), 'a', 'z'))
#define na_isalpha(X)							\
    ({									\
      const __typeof__(X) _x = (X);					\
      na_islower(_x) || na_isupper(_x);					\
    })
#define na_iscntrl(X)							\
    ({									\
      const __typeof__(X) _x = (X);					\
      _IN_RANGE(_x, 0, 0x1F) || _x == 0x7F;				\
    })
#define na_isalnum(X)							\
    ({									\
      const __typeof__(X) _y = (X);					\
      na_isalpha(_y) || na_isdigit(_y);					\
    })
#define na_isgraph(X)							\
    ({									\
      const __typeof__(X) _z = (X);					\
      na_isalnum(_z) || na_ispunct(_z);					\
    })
/* xxx why the 'x' below ??? this is probably wrong and incomplet */
#define na_isprint(X)							\
    ({									\
      const __typeof__(X) _s = (X);					\
      na_isgraph(_s) || _s == 'x';					\
    })
#define na_ispunct(X)							\
    ({									\
      const __typeof__(X) _t = (X);					\
      !na_isalnum(_t) && _t != ' ';					\
    })
#define na_isspace(X)							\
    ({                                                                  \
      const __typeof__(X) _x = (X);					\
      _x == ' ' || _x == '\t' || _x == '\v' || _x == '\n' || _x == '\r' \
                || _x == '\f';						\
    })
#define na_isxdigit(X)							\
    ({									\
      const __typeof__(X) _x = (X);					\
      na_isdigit(_x) || _IN_RANGE(_x, 'a', 'f') || _IN_RANGE(_x, 'A', 'F'); \
    })
#define na_tolower(X)							\
    ({									\
      const __typeof__(X) _x = (X);					\
      na_isupper(_x) ? _x|' ' : _x;					\
    })
#define na_toupper(X)							\
    ({									\
      const __typeof__(X) _x = (X);					\
      na_islower(_x) ? _x&~' ' : _x;					\
    })


const unsigned char my_ctype_ascii[1 + _CTYPE_NUM_CHARS + 8] = {
	0,
/*01*/	_C,	_C,	_C,	_C,	_C,	_C,	_C,	_C,
/*02*/	_C,	_C|_S,	_C|_S,	_C|_S,	_C|_S,	_C|_S,	_C,	_C,
/*03*/	_C,	_C,	_C,	_C,	_C,	_C,	_C,	_C,
/*04*/	_C,	_C,	_C,	_C,	_C,	_C,	_C,	_C,
/*05*/	_S|_B,	_P,	_P,	_P,	_P,	_P,	_P,	_P,
/*06*/	_P,	_P,	_P,	_P,	_P,	_P,	_P,	_P,
/*07*/	_N,	_N,	_N,	_N,	_N,	_N,	_N,	_N,
/*08*/	_N,	_N,	_P,	_P,	_P,	_P,	_P,	_P,
/*09*/	_P,	_U|_X,	_U|_X,	_U|_X,	_U|_X,	_U|_X,	_U|_X,	_U,
/*10*/	_U,	_U,	_U,	_U,	_U,	_U,	_U,	_U,
/*11*/	_U,	_U,	_U,	_U,	_U,	_U,	_U,	_U,
/*12*/	_U,	_U,	_U,	_P,	_P,	_P,	_P,	_P,
/*13*/	_P,	_L|_X,	_L|_X,	_L|_X,	_L|_X,	_L|_X,	_L|_X,	_L,
/*14*/	_L,	_L,	_L,	_L,	_L,	_L,	_L,	_L,
/*15*/	_L,	_L,	_L,	_L,	_L,	_L,	_L,	_L,
/*16*/	_L,	_L,	_L,	_P,	_P,	_P,	_P,	_C,

/*17*/	0,	0,	0,	0,	0,	0,	0,	0,
/*18*/	0,	0,	0,	0,	0,	0,	0,	0,
/*19*/	0,	0,	0,	0,	0,	0,	0,	0,
/*20*/	0,	0,	0,	0,	0,	0,	0,	0,
/*21*/	0,	0,	0,	0,	0,	0,	0,	0,
/*22*/	0,	0,	0,	0,	0,	0,	0,	0,
/*23*/	0,	0,	0,	0,	0,	0,	0,	0,
/*24*/	0,	0,	0,	0,	0,	0,	0,	0,
/*25*/	0,	0,	0,	0,	0,	0,	0,	0,
/*26*/	0,	0,	0,	0,	0,	0,	0,	0,
/*27*/	0,	0,	0,	0,	0,	0,	0,	0,
/*28*/	0,	0,	0,	0,	0,	0,	0,	0,
/*29*/	0,	0,	0,	0,	0,	0,	0,	0,
/*30*/	0,	0,	0,	0,	0,	0,	0,	0,
/*31*/	0,	0,	0,	0,	0,	0,	0,	0,
/*32*/	0,	0,	0,	0,	0,	0,	0,	0,
	0xE0,	0xE1,	0xE2,	0xE3,	0xE4,	0xE5,	0xE6,	0xE7
};

const unsigned char my_ctype_8859_1[1 + _CTYPE_NUM_CHARS + 8] = {
	0,								/* ch */
/*01*/	_C,	_C,	_C,	_C,	_C,	_C,	_C,	_C,	/*0x00*/
/*02*/	_C,	_C|_S,	_C|_S,	_C|_S,	_C|_S,	_C|_S,	_C,	_C,	/*0x08*/
/*03*/	_C,	_C,	_C,	_C,	_C,	_C,	_C,	_C,	/*0x10*/
/*04*/	_C,	_C,	_C,	_C,	_C,	_C,	_C,	_C,	/*0x18*/
/*05*/	_S|_B,	_P,	_P,	_P,	_P,	_P,	_P,	_P,	/*0x20*/
/*06*/	_P,	_P,	_P,	_P,	_P,	_P,	_P,	_P,	/*0x28*/
/*07*/	_N,	_N,	_N,	_N,	_N,	_N,	_N,	_N,	/*0x30*/
/*08*/	_N,	_N,	_P,	_P,	_P,	_P,	_P,	_P,	/*0x38*/
/*09*/	_P,	_U|_X,	_U|_X,	_U|_X,	_U|_X,	_U|_X,	_U|_X,	_U,	/*0x40*/
/*10*/	_U,	_U,	_U,	_U,	_U,	_U,	_U,	_U,	/*0x49*/
/*11*/	_U,	_U,	_U,	_U,	_U,	_U,	_U,	_U,	/*0x50*/
/*12*/	_U,	_U,	_U,	_P,	_P,	_P,	_P,	_P,	/*0x58*/
/*13*/	_P,	_L|_X,	_L|_X,	_L|_X,	_L|_X,	_L|_X,	_L|_X,	_L,	/*0x60*/
/*14*/	_L,	_L,	_L,	_L,	_L,	_L,	_L,	_L,	/*0x69*/
/*15*/	_L,	_L,	_L,	_L,	_L,	_L,	_L,	_L,	/*0x70*/
/*16*/	_L,	_L,	_L,	_P,	_P,	_P,	_P,	_C,	/*0x78*/

/*17*/	_C,	_C,	_C,	_C,	_C,	_C,	_C,	_C,	/*0x80*/
/*18*/	_C,	_C,	_C,	_C,	_C,	_C,	_C,	_C,	/*0x88*/
/*19*/	_C,	_C,	_C,	_C,	_C,	_C,	_C,	_C,	/*0x90*/
/*20*/	_C,	_C,	_C,	_C,	_C,	_C,	_C,	_C,	/*0x98*/
/*21*/	_S|_B,	_P,	_P,	_P,	_P,	_P,	_P,	_P,	/*0xA0*/
/*22*/	_P,	_P,	_P,	_P,	_P,	_P,	_P,	_P,	/*0xA8*/
/*23*/	_P,	_P,	_P,	_P,	_P,	_P,	_P,	_P,	/*0xB0*/
/*24*/	_P,	_P,	_P,	_P,	_P,	_P,	_P,	_P,	/*0xB8*/
/*25*/	_U,	_U,	_U,	_U,	_U,	_U,	_U,	_U,	/*0xC0*/
/*26*/	_U,	_U,	_U,	_U,	_U,	_U,	_U,	_U,	/*0xC8*/
/*27*/	_U,	_U,	_U,	_U,	_U,	_U,	_U,	_P,	/*0xD0*/
/*28*/	_U,	_U,	_U,	_U,	_U,	_U,	_U,	_L,	/*0xD8*/
/*29*/	_L,	_L,	_L,	_L,	_L,	_L,	_L,	_L,	/*0xE0*/
/*30*/	_L,	_L,	_L,	_L,	_L,	_L,	_L,	_L,	/*0xE8*/
/*31*/	_L,	_L,	_L,	_L,	_L,	_L,	_L,	_P,	/*0xF0*/
/*32*/	_L,	_L,	_L,	_L,	_L,	_L,	_L,	_L,	/*0xF8*/
	0xE0,	0xE1,	0xE2,	0xE3,	0xE4,	0xE5,	0xE6,	0xE7
};

const unsigned char *my_ctype = my_ctype_ascii;

const short my_tolower_ascii[1 + 256] = {
	EOF,
	0x00,	0x01,	0x02,	0x03,	0x04,	0x05,	0x06,	0x07,
	0x08,	0x09,	0x0a,	0x0b,	0x0c,	0x0d,	0x0e,	0x0f,
	0x10,	0x11,	0x12,	0x13,	0x14,	0x15,	0x16,	0x17,
	0x18,	0x19,	0x1a,	0x1b,	0x1c,	0x1d,	0x1e,	0x1f,
	0x20,	0x21,	0x22,	0x23,	0x24,	0x25,	0x26,	0x27,
	0x28,	0x29,	0x2a,	0x2b,	0x2c,	0x2d,	0x2e,	0x2f,
	0x30,	0x31,	0x32,	0x33,	0x34,	0x35,	0x36,	0x37,
	0x38,	0x39,	0x3a,	0x3b,	0x3c,	0x3d,	0x3e,	0x3f,
	0x40,	'a',	'b',	'c',	'd',	'e',	'f',	'g',
	'h',	'i',	'j',	'k',	'l',	'm',	'n',	'o',
	'p',	'q',	'r',	's',	't',	'u',	'v',	'w',
	'x',	'y',	'z',	0x5b,	0x5c,	0x5d,	0x5e,	0x5f,
	0x60,	0x61,	0x62,	0x63,	0x64,	0x65,	0x66,	0x67,
	0x68,	0x69,	0x6a,	0x6b,	0x6c,	0x6d,	0x6e,	0x6f,
	0x70,	0x71,	0x72,	0x73,	0x74,	0x75,	0x76,	0x77,
	0x78,	0x79,	0x7a,	0x7b,	0x7c,	0x7d,	0x7e,	0x7f,
	0x80,	0x81,	0x82,	0x83,	0x84,	0x85,	0x86,	0x87,
	0x88,	0x89,	0x8a,	0x8b,	0x8c,	0x8d,	0x8e,	0x8f,
	0x90,	0x91,	0x92,	0x93,	0x94,	0x95,	0x96,	0x97,
	0x98,	0x99,	0x9a,	0x9b,	0x9c,	0x9d,	0x9e,	0x9f,
	0xa0,	0xa1,	0xa2,	0xa3,	0xa4,	0xa5,	0xa6,	0xa7,
	0xa8,	0xa9,	0xaa,	0xab,	0xac,	0xad,	0xae,	0xaf,
	0xb0,	0xb1,	0xb2,	0xb3,	0xb4,	0xb5,	0xb6,	0xb7,
	0xb8,	0xb9,	0xba,	0xbb,	0xbc,	0xbd,	0xbe,	0xbf,
	0xc0,	0xc1,	0xc2,	0xc3,	0xc4,	0xc5,	0xc6,	0xc7,
	0xc8,	0xc9,	0xca,	0xcb,	0xcc,	0xcd,	0xce,	0xcf,
	0xd0,	0xd1,	0xd2,	0xd3,	0xd4,	0xd5,	0xd6,	0xd7,
	0xd8,	0xd9,	0xda,	0xdb,	0xdc,	0xdd,	0xde,	0xdf,
	0xe0,	0xe1,	0xe2,	0xe3,	0xe4,	0xe5,	0xe6,	0xe7,
	0xe8,	0xe9,	0xea,	0xeb,	0xec,	0xed,	0xee,	0xef,
	0xf0,	0xf1,	0xf2,	0xf3,	0xf4,	0xf5,	0xf6,	0xf7,
	0xf8,	0xf9,	0xfa,	0xfb,	0xfc,	0xfd,	0xfe,	0xff
};

const short my_tolower_8859_1[1 + 256] = {
	EOF,
	0x00,	0x01,	0x02,	0x03,	0x04,	0x05,	0x06,	0x07,
	0x08,	0x09,	0x0a,	0x0b,	0x0c,	0x0d,	0x0e,	0x0f,
	0x10,	0x11,	0x12,	0x13,	0x14,	0x15,	0x16,	0x17,
	0x18,	0x19,	0x1a,	0x1b,	0x1c,	0x1d,	0x1e,	0x1f,
	0x20,	0x21,	0x22,	0x23,	0x24,	0x25,	0x26,	0x27,
	0x28,	0x29,	0x2a,	0x2b,	0x2c,	0x2d,	0x2e,	0x2f,
	0x30,	0x31,	0x32,	0x33,	0x34,	0x35,	0x36,	0x37,
	0x38,	0x39,	0x3a,	0x3b,	0x3c,	0x3d,	0x3e,	0x3f,
	0x40,	'a',	'b',	'c',	'd',	'e',	'f',	'g',
	'h',	'i',	'j',	'k',	'l',	'm',	'n',	'o',
	'p',	'q',	'r',	's',	't',	'u',	'v',	'w',
	'x',	'y',	'z',	0x5b,	0x5c,	0x5d,	0x5e,	0x5f,
	0x60,	0x61,	0x62,	0x63,	0x64,	0x65,	0x66,	0x67,
	0x68,	0x69,	0x6a,	0x6b,	0x6c,	0x6d,	0x6e,	0x6f,
	0x70,	0x71,	0x72,	0x73,	0x74,	0x75,	0x76,	0x77,
	0x78,	0x79,	0x7a,	0x7b,	0x7c,	0x7d,	0x7e,	0x7f,
	0x80,	0x81,	0x82,	0x83,	0x84,	0x85,	0x86,	0x87,
	0x88,	0x89,	0x8a,	0x8b,	0x8c,	0x8d,	0x8e,	0x8f,
	0x90,	0x91,	0x92,	0x93,	0x94,	0x95,	0x96,	0x97,
	0x98,	0x99,	0x9a,	0x9b,	0x9c,	0x9d,	0x9e,	0x9f,
	0xa0,	0xa1,	0xa2,	0xa3,	0xa4,	0xa5,	0xa6,	0xa7,
	0xa8,	0xa9,	0xaa,	0xab,	0xac,	0xad,	0xae,	0xaf,
	0xb0,	0xb1,	0xb2,	0xb3,	0xb4,	0xb5,	0xb6,	0xb7,
	0xb8,	0xb9,	0xba,	0xbb,	0xbc,	0xbd,	0xbe,	0xbf,
/**/	0xe0,	0xe1,	0xe2,	0xe3,	0xe4,	0xe5,	0xe6,	0xe7,
/**/	0xe8,	0xe9,	0xea,	0xeb,	0xec,	0xed,	0xee,	0xef,
/**/	0xf0,	0xf1,	0xf2,	0xf3,	0xf4,	0xf5,	0xf6,	0xd7,
/**/	0xf8,	0xf9,	0xfa,	0xfb,	0xfc,	0xfd,	0xfe,	0xdf,
	0xe0,	0xe1,	0xe2,	0xe3,	0xe4,	0xe5,	0xe6,	0xe7,
	0xe8,	0xe9,	0xea,	0xeb,	0xec,	0xed,	0xee,	0xef,
	0xf0,	0xf1,	0xf2,	0xf3,	0xf4,	0xf5,	0xf6,	0xf7,
	0xf8,	0xf9,	0xfa,	0xfb,	0xfc,	0xfd,	0xfe,	0xff
};

const short *my_tolower_tab = my_tolower_ascii;

/*
 * XXX these demonstrate why implementation as functions won't ever work when
 * passed a signed char with a value of -1 (i.e. 0xFF for CHAR_BIT==8) because
 * of course the sign will be extended and the resulting value will be seen as
 * EOF internally.  Even if they were declared to take an "unsigned int", sign
 * extension would still happen and the -1 signed char as a parameter would
 * still be indistinguishable from EOF in the function body.
 */

int my_isalpha(int);
int my_tolower(int);

int
my_isalpha(int ui)
{
	const unsigned char ch = (unsigned char) ui;

	printf("my_isalpha(): ui = 0x%x, ch = 0x%x, (unsigned char) ch = 0x%x, my_ct = 0x%x\n",
	       ui,
	       (unsigned int) ch,
	       (unsigned int) ((unsigned char) ch),
	       (unsigned int) ((my_ctype + 1)[(unsigned char) ch]));
	/* XXX this will always fail if called with a (signed char) 0xFF */
	if (ui == EOF) {
		printf("my_isalpha(): returning 0 for EOF\n");
		return 0;
	}
	return ((int) ((my_ctype + 1)[(unsigned char) ch] & (_U|_L)));
}

int
my_tolower(int ui)
{
	const unsigned char ch = (unsigned char) ui;

	printf("my_tolower(): ui = 0x%x, ch = 0x%x, (unsigned char) ch = 0x%x\n",
	       ui,
	       (unsigned int) ch,
	       (unsigned int) ((unsigned char) ch));
	/* XXX this will always fail if called with a (signed char) 0xFF */
	if (ui == EOF) {
		printf("my_tolower(): returning EOF for EOF\n");
		return EOF;
	}
	return ((int) ((my_tolower_tab + 1)[(unsigned char) ch]));
}

#define CORRECTP(expr)	((expr) ? " [correct]" : "[ERROR:WRONG]")

/*
 * N.B.:  XXX WARNING!!! XXX
 *
 * Apparently statement expressions in the same block share the same identifier
 * namespace!
 */
static void
skipwhitespace(const char **sp)
{
    char _xc, _comment = 0;

    while ((_xc = *(*sp))) {
	if (_xc == '(') {
	    _comment = 1;
	    (*sp)++;
	    while ((_comment && (_xc = *(*sp)))) {
		(*sp)++;
		if (_xc == '\\' && *(*sp)) (*sp)++;
		else if (_xc == '(') _comment++;
		else if (_xc == ')') _comment--;
	    }
	    (*sp)--;
	} else if (! isspace(_xc)) {
		switch (_xc) {
		case ' ':
		case '\f':
		case '\n':
		case '\r':
		case '\t':
		case '\v':
			printf("WRONG: is still whitespace: '%c'[0x%x] (returned 0x%x)\n",
			       _xc, _xc, isspace(_xc));
			break;
		default:
			break;
		}
		break;
	}
	(*sp)++;
    }
}

/*
 * N.B.:  This version may reveal compiler issues with shadowing in statement
 * expressions because it uses the same local variable ('_ctype_uc_') as the
 * MY_ISSPACE() macro.
 */
static void
skipwhitespace_broken(const char **sp)
{
    char _ctype_uc_, _comment = 0;		/* xxx _uc_ is overwritten by MY_ISSPACE()'s _uc_ */

    while ((_ctype_uc_ = *(*sp))) {
	if (_ctype_uc_ == '(') {
	    _comment = 1;
	    (*sp)++;
	    while ((_comment && (_ctype_uc_ = *(*sp)))) {
		(*sp)++;
		if (_ctype_uc_ == '\\' && *(*sp)) (*sp)++;
		else if (_ctype_uc_ == '(') _comment++;
		else if (_ctype_uc_ == ')') _comment--;
	    }
	    (*sp)--;
	} else if (! MY_ISSPACE(_ctype_uc_)) {
		/*
		 * GCC from the line above:
tctype.c:449: warning: declaration of '_ctype_uc_' shadows a previous local
tctype.c:436: warning: shadowed declaration is here
		 *
		 * Clang (10.x) says:
tctype.c:449:15: warning: declaration shadows a local variable [-Wshadow]
        } else if (! MY_ISSPACE(_ctype_uc_)) {
                     ^
tctype.c:77:23: note: expanded from macro 'MY_ISSPACE'
#define MY_ISSPACE(c)   MY__IS_CTYPER(c, _S)
                        ^
tctype.c:70:23: note: expanded from macro 'MY__IS_CTYPER'
                const unsigned char _ctype_uc_ = (unsigned char) _ctype_ui_; \
                                    ^
tctype.c:436:10: note: previous declaration is here
    char _ctype_uc_, _comment = 0;              (* xxx _uc_ is overwritten by MY_ISSPACE()'s _uc_ *)
         ^
		 */
		switch (_ctype_uc_) {
		case ' ':
		case '\f':
		case '\n':
		case '\r':
		case '\t':
		case '\v':
			printf("WRONG: is still whitespace: '%c'[0x%x] (returned 0x%x)\n",
			       _ctype_uc_, _ctype_uc_, isspace(_ctype_uc_));
			break;
		default:
			break;
		}
		break;
	}
	(*sp)++;
    }
}

static void
test_skipwhitespace(const char *foo)
{
	printf("\nbefore: '%s'\n", foo);

	skipwhitespace(&foo);

	printf("after:  '%s'\n", foo);

	switch (*foo) {
	case ' ':
	case '\f':
	case '\n':
	case '\r':
	case '\t':
	case '\v':
		printf("WRONG: is still whitespace at start: '%c'[0x%x]\n",
		       *foo, *foo);
		break;
	default:
		break;
	}

	return;
}

static void
test_skipwhitespace_broken(const char *foo)
{
	printf("\nbefore: '%s'\n", foo);

	skipwhitespace_broken(&foo);

	printf("after:  '%s'\n", foo);

	switch (*foo) {
	case ' ':
	case '\f':
	case '\n':
	case '\r':
	case '\t':
	case '\v':
		printf("WRONG: is still whitespace at start: '%c'[0x%x]\n",
		       *foo, *foo);
		break;
	default:
		break;
	}

	return;
}


void t_in_cond(const void *, size_t);

/* test a conditional expression inside a macro call */
void
t_in_cond(const void *buf,
	  size_t len)
{
	const unsigned char *p = buf;
	size_t x;

	printf("printable from p[%s] = '", p);

	for (x = 0; x < len / 4; x += 4) { /* xxx warning: comparison between signed and unsigned integer expressions [-Wsign-compare] */
		size_t y;

		for (y = 0; x + y < len; y++) { /* warning: comparison between signed and unsigned integer expressions [-Wsign-compare] */
			fprintf(stdout, "%c",
				MY_ISPRINT(p[x + y]) ? p[x + y] : '.');
		}
	}
	puts("'");
}


static int
my_strcasecmp(const char *s1, const char *s2)
{
	const unsigned char *us1 = (const unsigned char *)s1,
			*us2 = (const unsigned char *)s2;

	while (MY_TOLOWER(*us1) == MY_TOLOWER(*us2++))
		if (*us1++ == '\0')
			return (0);
	return (MY_TOLOWER(*us1) - MY_TOLOWER(*--us2));
}


int main(void);

int
main()
{
	int i = EOF;
	long int l = EOF;
	char c = EOF;
	signed char one = '1';
	unsigned int ui = (unsigned int) EOF;
	unsigned long int ul = (unsigned long) EOF;
	unsigned char uc = (unsigned char) EOF;
	signed char sc = (signed char) EOF;
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
	printf("char bits = %d %s CHAR_BIT [%d]\n", char_bits,
	       char_bits == CHAR_BIT ? "==" : "!=", (int) CHAR_BIT);

	printf("CHAR_BIT = %d\n", CHAR_BIT);
	printf("char mask = 0x%x\n", ~(UINT_MAX << CHAR_BIT));
	printf("sizeof(EOF) = %lu\n", (unsigned long) sizeof(EOF));

#if 0					/* xxx default is my_ctype_ascii */
	my_ctype = my_ctype_8859_1;
#endif

	(void) isalpha(i);
	(void) isalpha(l);		/* passing argument 1 of 'isalpha' with different width due to prototype */
	(void) isalpha(c);		/* XXX on unmodified NetBSD: array subscript has type 'char' */
	(void) isalpha(ui);		/* passing argument 1 of 'isalpha' as signed due to prototype */
	(void) isalpha(ul);		/* passing argument 1 of 'isalpha' with different width due to prototype */
	(void) isalpha(uc);
	(void) isalpha(sc);
	(void) my_isalpha(i);
	(void) my_isalpha(l);;		/* passing argument 1 of 'my_isalpha' with different width due to prototype */
	(void) my_isalpha(c);
	(void) my_isalpha(ui);		/* passing argument 1 of 'my_isalpha' as signed due to prototype */
	(void) my_isalpha(ul);		/* passing argument 1 of 'my_isalpha' as signed due to prototype | with different width due to prototype */
	(void) my_isalpha(uc);
	(void) my_isalpha(sc);
	(void) _tolower(i);
	(void) _tolower(l);;		/* passing argument 1 of '__tolower' with different width due to prototype */
	(void) _tolower(c);
	(void) _tolower(ui);		/* passing argument 1 of '__tolower' as signed due to prototype */
	(void) _tolower(ul);		/* passing argument 1 of '__tolower' with different width due to prototype */
	(void) _tolower(uc);
	(void) _tolower(sc);
	(void) tolower(i);
	(void) tolower(l);		/* passing argument 1 of 'tolower' with different width due to prototype */
	(void) tolower(c);		/* XXX on unmodified NetBSD: array subscript has type 'char' */
	(void) tolower(ui);		/* passing argument 1 of 'tolower' as signed due to prototype */
	(void) tolower(ul);		/* passing argument 1 of 'tolower' with different width due to prototype */
	(void) tolower(uc);
	(void) tolower(sc);
	(void) my_tolower(i);
	(void) my_tolower(l);		/* passing argument 1 of 'my_tolower' with different width due to prototype */
	(void) my_tolower(c);
	(void) my_tolower(ui);		/* passing argument 1 of 'my_tolower' as signed due to prototype */
	(void) my_tolower(ul);		/* passing argument 1 of 'my_tolower' as signed due to prototype | with different width due to prototype */
	(void) my_tolower(uc);
	(void) my_tolower(sc);

	(void) isalpha(EOF);		/* negative int implicit convertion to unsigned */
	(void) _tolower(EOF);
	(void) tolower(EOF);		/* negative int implicit convertion to unsigned */

	printf("\nAll variable initially set to EOF = 0x%x\n", EOF);
	printf("(unsigned int) EOF = 0x%x\n", (unsigned int) EOF);
	putchar('\n');

	printf("(int) (unsigned char) EOF = 0x%x\n", (int) (unsigned char) EOF);
	printf("(unsigned int) (unsigned char) EOF = 0x%x\n", (unsigned int) (unsigned char) EOF);
	printf("(unsigned int) ((unsigned int) EOF & _CTYPE_MASK) = 0x%x\n", (unsigned int) ((unsigned int) EOF & _CTYPE_MASK));
	i = 0xFF;
	printf("(unsigned int) i = 0xFF = 0x%x\n", (unsigned int) i);
	i = EOF;
	printf("(unsigned int) i = EOF = 0x%x\n", (unsigned int) ui);
	sc = (signed char) 0xFF;
	printf("(unsigned int) sc = 0xFF = 0x%x\n", (unsigned int) sc);
	putchar('\n');

	printf("my+1[EOF & _CTYPE_MASK] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(EOF & _CTYPE_MASK)]); /* negative int implicit convertion to unsigned */
	printf("my+1[(unsigned char)EOF] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(unsigned char)(EOF)]);
	printf("my+1[EOF] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(EOF)]);
	putchar('\n');

	printf("my+1[c & _CTYPE_MASK] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(c & _CTYPE_MASK)]);
	printf("my+1[(unsigned char)c] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(unsigned char)(c)]);
	printf("my+1[c] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(c)]); /* XXX subscript has type char, not unsigned char */
	putchar('\n');

	printf("my+1[i & _CTYPE_MASK] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(i & _CTYPE_MASK)]);
	printf("my+1[(unsigned char)i] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(unsigned char)(i)]);
	printf("my+1[i] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(i)]);
	putchar('\n');

	printf("my+1[l & _CTYPE_MASK] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(l & _CTYPE_MASK)]);
	printf("my+1[(unsigned char)l] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(unsigned char)(i)]);
	printf("my+1[l] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(l)]);
	putchar('\n');

	printf("my+1[uc & _CTYPE_MASK] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(uc & _CTYPE_MASK)]);
	printf("my+1[(unsigned char)uc] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(unsigned char)(uc)]);
	printf("my+1[uc] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(uc)]);
	putchar('\n');

	printf("my+1[sc & _CTYPE_MASK] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(sc & _CTYPE_MASK)]);
	printf("my+1[(unsigned char)sc] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(unsigned char)(sc)]);
	printf("my+1[sc] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(sc)]);
	putchar('\n');

	printf("my+1[0xFFU] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[0xFFU]);
	printf("my[0xFFU] = 0x%x\n",
	       (unsigned int)(my_ctype)[0xFFU]);
	putchar('\n');

	printf("isalpha(EOF) = 0x%x %s\n", isalpha(EOF), CORRECTP(!isalpha(EOF)));
	printf("my_isalpha(EOF) = 0x%x %s\n", my_isalpha(EOF), CORRECTP(!my_isalpha(EOF)));
	printf("na_isalpha(EOF) = 0x%x %s\n", na_isalpha(EOF), CORRECTP(!na_isalpha(EOF)));
	printf("islower(EOF) = 0x%x %s\n", islower(EOF), CORRECTP(!islower(EOF)));
	printf("isupper(EOF) = 0x%x %s\n", isupper(EOF), CORRECTP(!isupper(EOF)));
	putchar('\n');

	printf("isalpha('a') = 0x%x %s\n", isalpha('a'), CORRECTP(isalpha('a')));
	printf("my_isalpha('a') = 0x%x %s\n", my_isalpha('a'), CORRECTP(my_isalpha('a')));
	printf("na_isalpha('a') = 0x%x %s\n", na_isalpha('a'), CORRECTP(na_isalpha('a')));
	printf("islower('a') = 0x%x %s\n", islower('a'), CORRECTP(islower('a')));
	printf("isupper('a') = 0x%x %s\n", isupper('a'), CORRECTP(!isupper('a')));
	putchar('\n');

	printf("isalpha('2') = 0x%x %s\n", isalpha('2'), CORRECTP(!isalpha('2')));
	printf("my_isalpha('2') = 0x%x %s\n", my_isalpha('2'), CORRECTP(!my_isalpha('2')));
	printf("na_isalpha('2') = 0x%x %s\n", na_isalpha('2'), CORRECTP(!na_isalpha('2')));
	printf("islower('2') = 0x%x %s\n", islower('2'), CORRECTP(!islower('2')));
	printf("isupper('2') = 0x%x %s\n", isupper('2'), CORRECTP(!isupper('2')));
	putchar('\n');

	printf("isdigit('2') = 0x%x %s\n", isdigit('2'), CORRECTP(isdigit('2')));
	printf("MY_ISDIGIT('2') = 0x%x %s\n", MY_ISDIGIT('2'), CORRECTP(MY_ISDIGIT('2')));
	printf("na_isdigit('2') = 0x%x %s\n", na_isdigit('2'), CORRECTP(na_isdigit('2')));
	putchar('\n');

	printf("isdigit('a') = 0x%x %s\n", isdigit('a'), CORRECTP(!isdigit('a')));
	printf("MY_ISDIGIT('a') = 0x%x %s\n", MY_ISDIGIT('a'), CORRECTP(!MY_ISDIGIT('a')));
	printf("na_isdigit('a') = 0x%x %s\n", na_isdigit('a'), CORRECTP(!na_isdigit('a')));
	putchar('\n');

	printf("isdigit('\\0') = 0x%x %s\n", isdigit('\0'), CORRECTP(!isdigit('\0')));
	printf("MY_ISDIGIT('\\0') = 0x%x %s\n", MY_ISDIGIT('\0'), CORRECTP(!MY_ISDIGIT('\0')));
	printf("na_isdigit('\\0') = 0x%x %s\n", na_isdigit('\0'), CORRECTP(!na_isdigit('\0')));
	putchar('\n');

	printf("isdigit('1') = 0x%x %s\n", isdigit(one), CORRECTP(isdigit(one)));
	printf("MY_ISDIGIT('1') = 0x%x %s\n", MY_ISDIGIT(one), CORRECTP(MY_ISDIGIT(one)));
	printf("na_isdigit('1') = 0x%x %s\n", na_isdigit(one), CORRECTP(na_isdigit(one)));
	putchar('\n');

	printf("isalnum(0) = 0x%x %s\n", isalnum(0), CORRECTP(!isalnum(0)));
	printf("MY_ISALNUM(0) = 0x%x %s\n", MY_ISALNUM(0), CORRECTP(!MY_ISALNUM(0)));
	printf("na_isalnum(0) = 0x%x %s\n", na_isalnum(0), CORRECTP(!na_isalnum(0)));
	putchar('\n');

	printf("isalnum('a') = 0x%x %s\n", isalnum('a'), CORRECTP(isalnum('a')));
	printf("MY_ISALNUM('a') = 0x%x %s\n", MY_ISALNUM('a'), CORRECTP(MY_ISALNUM('a')));
	printf("na_isalnum('a') = 0x%x %s\n", na_isalnum('a'), CORRECTP(na_isalnum('a')));
	putchar('\n');

	printf("isalnum('1') = 0x%x %s\n", isalnum('1'), CORRECTP(isalnum('1')));
	printf("MY_ISALNUM('1') = 0x%x %s\n", MY_ISALNUM('1'), CORRECTP(MY_ISALNUM('1')));
	printf("na_isalnum('1') = 0x%x %s\n", na_isalnum('1'), CORRECTP(na_isalnum('1')));
	putchar('\n');

	printf("isalnum(' ') = 0x%x %s\n", isalnum(' '), CORRECTP(!isalnum(' ')));
	printf("MY_ISALNUM(' ') = 0x%x %s\n", MY_ISALNUM(' '), CORRECTP(!MY_ISALNUM(' ')));
	printf("na_isalnum(' ') = 0x%x %s\n", na_isalnum(' '), CORRECTP(!na_isalnum(' ')));
	putchar('\n');

	printf("islower(SP) = 0x%x %s\n", islower(EOF), CORRECTP(!islower(EOF)));
	printf("isupper(SP) = 0x%x %s\n", isupper(EOF), CORRECTP(!isupper(EOF)));
	putchar('\n');

	printf("isspace(0) = 0x%x %s\n", isspace(0), CORRECTP(!isspace(0)));
	printf("na_isspace(0) = 0x%x %s\n", na_isspace(0), CORRECTP(!na_isspace(0)));
	printf("MY_ISSPACE(0) = 0x%x %s\n", MY_ISSPACE(0), CORRECTP(!MY_ISSPACE(0)));

	printf("isspace('a') = 0x%x %s\n", isspace('a'), CORRECTP(!isspace('a')));
	printf("na_isspace('a') = 0x%x %s\n", na_isspace('a'), CORRECTP(!na_isspace('a')));
	printf("MY_ISSPACE('a') = 0x%x %s\n", MY_ISSPACE('a'), CORRECTP(!MY_ISSPACE('a')));

	printf("isspace('1') = 0x%x %s\n", isspace('1'), CORRECTP(!isspace('1')));
	printf("na_isspace('1') = 0x%x %s\n", na_isspace('1'), CORRECTP(!na_isspace('1')));
	printf("MY_ISSPACE('1') = 0x%x %s\n", MY_ISSPACE('1'), CORRECTP(!MY_ISSPACE('1')));

	printf("isspace('.') = 0x%x %s\n", isspace('.'), CORRECTP(!isspace('.')));
	printf("na_isspace('.') = 0x%x %s\n", na_isspace('.'), CORRECTP(!na_isspace('.')));
	printf("MY_ISSPACE('.') = 0x%x %s\n", MY_ISSPACE('.'), CORRECTP(!MY_ISSPACE('.')));

	printf("isspace(EOF) = 0x%x %s\n", isspace(EOF), CORRECTP(!isspace(EOF)));
	printf("na_isspace(EOF) = 0x%x %s\n", na_isspace(EOF), CORRECTP(!na_isspace(EOF)));
	printf("MY_ISSPACE(EOF) = 0x%x %s\n", MY_ISSPACE(EOF), CORRECTP(!MY_ISSPACE(EOF)));
	putchar('\n');

	sc = ' ';
	printf("isspace(sc = '%c'[0x%x]) = 0x%x %s\n", sc, sc, isspace(sc), CORRECTP(isspace(sc)));
	printf("na_isspace(sc = '%c'[0x%x]) = 0x%x %s\n", sc, sc, na_isspace(sc), CORRECTP(na_isspace(sc)));
	printf("MY_ISSPACE(sc = '%c'[0x%x]) = 0x%x %s\n", sc, sc, MY_ISSPACE(sc), CORRECTP(MY_ISSPACE(sc)));

	uc = ' ';
	printf("isspace(uc = '%c'[0x%x]) = 0x%x %s\n", uc, uc, isspace(uc), CORRECTP(isspace(uc)));
	printf("na_isspace(uc = '%c'[0x%x]) = 0x%x %s\n", uc, uc, na_isspace(uc), CORRECTP(na_isspace(uc)));
	printf("MY_ISSPACE(uc = '%c'[0x%x]) = 0x%x %s\n", uc, uc, MY_ISSPACE(uc), CORRECTP(MY_ISSPACE(uc)));
	putchar('\n');

	sc = 'a';
	printf("isspace(sc = '%c'[0x%x]) = 0x%x %s\n", sc, sc, isspace(sc), CORRECTP(!isspace(sc)));
	printf("na_isspace(sc = '%c'[0x%x]) = 0x%x %s\n", sc, sc, na_isspace(sc), CORRECTP(!na_isspace(sc)));
	printf("MY_ISSPACE(sc = '%c'[0x%x]) = 0x%x %s\n", sc, sc, MY_ISSPACE(sc), CORRECTP(!MY_ISSPACE(sc)));

	uc = 'a';
	printf("isspace(uc = '%c'[0x%x]) = 0x%x %s\n", uc, uc, isspace(uc), CORRECTP(!isspace(uc)));
	printf("na_isspace(uc = '%c'[0x%x]) = 0x%x %s\n", uc, uc, na_isspace(uc), CORRECTP(!na_isspace(uc)));
	printf("MY_ISSPACE(uc = '%c'[0x%x]) = 0x%x %s\n", uc, uc, MY_ISSPACE(uc), CORRECTP(!MY_ISSPACE(uc)));
	putchar('\n');

	printf("isspace(SP) = 0x%x %s\n", isspace(' '), CORRECTP(isspace(' ')));
	printf("isspace(32) = 0x%x %s\n", isspace(32), CORRECTP(isspace(32)));
	printf("isspace(NL) = 0x%x %s\n", isspace('\n'), CORRECTP(isspace('\n')));
	printf("isspace(10) = 0x%x %s\n", isspace(10), CORRECTP(isspace(10)));
	printf("isspace(CR) = 0x%x %s\n", isspace('\r'), CORRECTP(isspace('\r')));
	printf("isspace(13) = 0x%x %s\n", isspace(13), CORRECTP(isspace(13)));
	printf("isspace(TAB) = 0x%x %s\n", isspace('\t'), CORRECTP(isspace('\t')));
	printf("isspace(09) = 0x%x %s\n", isspace(9), CORRECTP(isspace(9)));
	printf("isspace(VT) = 0x%x %s\n", isspace('\v'), CORRECTP(isspace('\v')));
	printf("isspace(11) = 0x%x %s\n", isspace(11), CORRECTP(isspace(11)));
	printf("isspace(FF) = 0x%x %s\n", isspace('\f'), CORRECTP(isspace('\f')));
	printf("isspace(12) = 0x%x %s\n", isspace(12), CORRECTP(isspace(12)));
	putchar('\n');

	printf("MY_ISSPACE(SP) = 0x%x %s\n", MY_ISSPACE(' '), CORRECTP(MY_ISSPACE(' ')));
	printf("MY_ISSPACE(32) = 0x%x %s\n", MY_ISSPACE(32), CORRECTP(MY_ISSPACE(32)));
	printf("MY_ISSPACE(NL) = 0x%x %s\n", MY_ISSPACE('\n'), CORRECTP(MY_ISSPACE('\n')));
	printf("MY_ISSPACE(10) = 0x%x %s\n", MY_ISSPACE(10), CORRECTP(MY_ISSPACE(10)));
	printf("MY_ISSPACE(CR) = 0x%x %s\n", MY_ISSPACE('\r'), CORRECTP(MY_ISSPACE('\r')));
	printf("MY_ISSPACE(13) = 0x%x %s\n", MY_ISSPACE(13), CORRECTP(MY_ISSPACE(13)));
	printf("MY_ISSPACE(TAB) = 0x%x %s\n", MY_ISSPACE('\t'), CORRECTP(MY_ISSPACE('\t')));
	printf("MY_ISSPACE(9) = 0x%x %s\n", MY_ISSPACE(9), CORRECTP(MY_ISSPACE(9)));
	printf("MY_ISSPACE(VT) = 0x%x %s\n", MY_ISSPACE('\v'), CORRECTP(MY_ISSPACE('\v')));
	printf("MY_ISSPACE(11) = 0x%x %s\n", MY_ISSPACE(11), CORRECTP(MY_ISSPACE(11)));
	printf("MY_ISSPACE(FF) = 0x%x %s\n", MY_ISSPACE('\f'), CORRECTP(MY_ISSPACE('\f')));
	printf("MY_ISSPACE(12) = 0x%x %s\n", MY_ISSPACE(12), CORRECTP(MY_ISSPACE(12)));
	putchar('\n');

	printf("na_isspace(SP) = 0x%x %s\n", na_isspace(' '), CORRECTP(na_isspace(' ')));
	printf("na_isspace(32) = 0x%x %s\n", na_isspace(32), CORRECTP(na_isspace(32)));
	printf("na_isspace(NL) = 0x%x %s\n", na_isspace('\n'), CORRECTP(na_isspace('\n')));
	printf("na_isspace(10) = 0x%x %s\n", na_isspace(10), CORRECTP(na_isspace(10)));
	printf("na_isspace(CR) = 0x%x %s\n", na_isspace('\r'), CORRECTP(na_isspace('\r')));
	printf("na_isspace(13) = 0x%x %s\n", na_isspace(13), CORRECTP(na_isspace(13)));
	printf("na_isspace(TAB) = 0x%x %s\n", na_isspace('\t'), CORRECTP(na_isspace('\t')));
	printf("na_isspace(9) = 0x%x %s\n", na_isspace(9), CORRECTP(na_isspace(9)));
	printf("na_isspace(VT) = 0x%x %s\n", na_isspace('\v'), CORRECTP(na_isspace('\v')));
	printf("na_isspace(11) = 0x%x %s\n", na_isspace(11), CORRECTP(na_isspace(11)));
	printf("na_isspace(FF) = 0x%x %s\n", na_isspace('\f'), CORRECTP(na_isspace('\f')));
	printf("na_isspace(12) = 0x%x %s\n", na_isspace(12), CORRECTP(na_isspace(12)));
	putchar('\n');

	sc = (signed char) ('A' | 0xA0); /* 0xE1, small a, acute accent */
	printf("0xE1, small a, acute accent: %c\n", sc);
	printf("sc = 0x%x\n", (int)(unsigned char) sc);
	printf("my+1[sc & _CTYPE_MASK] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(sc & _CTYPE_MASK)]);
	printf("my+1[(unsigned char)sc] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(unsigned char)(sc)]);
	printf("my+1[sc] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(sc)]);
	printf("isalpha(sc) = 0x%x %s\n", isalpha(sc), CORRECTP(!isalpha(sc)));
	printf("isalpha(uc) = 0x%x %s\n", isalpha(uc), CORRECTP(!isalpha(uc)));
	printf("my_isalpha(sc) = 0x%x %s\n", my_isalpha(sc), CORRECTP(!my_isalpha(sc)));
	printf("my_isalpha(uc) = 0x%x %s\n", my_isalpha(uc), CORRECTP(!my_isalpha(uc)));
	printf("islower(sc) = 0x%x %s\n", islower(sc), CORRECTP(!islower(sc)));
	printf("islower(uc) = 0x%x %s\n", islower(uc), CORRECTP(!islower(uc)));
	printf("isupper(sc) = 0x%x %s\n", isupper(sc), CORRECTP(!isupper(sc)));
	printf("isupper(uc) = 0x%x %s\n", isupper(uc), CORRECTP(!isupper(uc)));
	putchar('\n');

	sc = (signed char) ('A' | 0xA0); /* 0xE1, small a, acute accent */
	printf("0xE1, small a, acute accent: %c\n", sc);
	printf("sc = 0x%x\n", (int)(unsigned char) sc);
	printf("my+1[sc & _CTYPE_MASK] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(sc & _CTYPE_MASK)]);
	printf("my+1[(unsigned char)sc] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(unsigned char)(sc)]);
	printf("my+1[sc] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(sc)]);
	printf("isalpha(sc) = 0x%x %s\n", isalpha(sc), CORRECTP(!isalpha(sc)));
	printf("isalpha(uc) = 0x%x %s\n", isalpha(uc), CORRECTP(!isalpha(uc)));
	printf("my_isalpha(sc) = 0x%x %s\n", my_isalpha(sc), CORRECTP(!my_isalpha(sc)));
	printf("my_isalpha(uc) = 0x%x %s\n", my_isalpha(uc), CORRECTP(!my_isalpha(uc)));
	printf("islower(sc) = 0x%x %s\n", islower(sc), CORRECTP(!islower(sc)));
	printf("islower(uc) = 0x%x %s\n", islower(uc), CORRECTP(!islower(uc)));
	printf("isupper(sc) = 0x%x %s\n", isupper(sc), CORRECTP(!isupper(sc)));
	printf("isupper(uc) = 0x%x %s\n", isupper(uc), CORRECTP(!isupper(uc)));
	putchar('\n');

	setlocale(LC_ALL, "en_US.ISO8859-1");
	my_ctype = my_ctype_8859_1;
	my_tolower_tab = my_tolower_8859_1;
	printf("setlocale(LC_ALL, \"en_US.ISO8859-1\");\n");

	sc = (signed char) ('A' | 0xA0); /* 0xE1, small a, acute accent */
	uc = 'A' | 0xA0;		/* 0xE1, small a, acute accent */
	printf("0xE1, small a, acute accent: SC=%c\n", sc);
	printf("0xE1, small a, acute accent: UC=%c\n", uc);
	printf("sc = 0x%x ('%c')\n", (unsigned int)(unsigned char) sc, sc);
	printf("uc = 0x%x ('%c')\n", (unsigned int) uc, uc);
	printf("my+1[sc & _CTYPE_MASK] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(sc & _CTYPE_MASK)]);
	printf("my+1[(unsigned char)sc] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(unsigned char)(sc)]);
	printf("my+1[sc] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(sc)]);
	printf("(my+1)[EOF] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(EOF)]);
	putchar('\n');

	printf("my+1[EOF & _CTYPE_MASK] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(EOF & _CTYPE_MASK)]); /* negative int implicit convertion to unsigned */
	printf("my+1[(unsigned char)EOF] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(unsigned char)(EOF)]);
	printf("my+1[EOF] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(EOF)]);
	putchar('\n');

	printf("my+1[c & _CTYPE_MASK] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(c & _CTYPE_MASK)]);
	printf("my+1[(unsigned char)c] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(unsigned char)(c)]);
	printf("my+1[c] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(c)]); /* XXX subscript has type char, not unsigned char */
	putchar('\n');

	printf("my+1[i & _CTYPE_MASK] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(i & _CTYPE_MASK)]);
	printf("my+1[(unsigned char)i] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(unsigned char)(i)]);
	printf("my+1[i] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(i)]);
	putchar('\n');

	printf("my+1[l & _CTYPE_MASK] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(l & _CTYPE_MASK)]);
	printf("my+1[(unsigned char)l] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(unsigned char)(i)]);
	printf("my+1[l] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(l)]);
	putchar('\n');

	printf("my+1[uc & _CTYPE_MASK] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(uc & _CTYPE_MASK)]);
	printf("my+1[(unsigned char)uc] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(unsigned char)(uc)]);
	printf("my+1[uc] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(uc)]);
	putchar('\n');

	printf("my+1[sc & _CTYPE_MASK] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(sc & _CTYPE_MASK)]);
	printf("my+1[(unsigned char)sc] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(unsigned char)(sc)]);
	printf("my+1[sc] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(sc)]);
	putchar('\n');

	printf("my+1[0xFFU] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[0xFFU]);
	printf("my[0xFFU] = 0x%x\n",
	       (unsigned int)(my_ctype)[0xFFU]);
	putchar('\n');

	/* XXX make sure we're in the C locale */
	setlocale(LC_ALL, "C");
	my_ctype = my_ctype_ascii;
	my_tolower_tab = my_tolower_ascii;
	printf("setlocale(LC_ALL, \"C\");\n");

	sc = (signed char) 0xE1;	/* small a, acute accent */
	uc = 0xE1;			/* small a, acute accent */
	i = 0xE1;			/* small a, acute accent */
	printf("0xE1, small a, acute accent: SC=%c\n", sc);
	printf("0xE1, small a, acute accent: UC=%c\n", uc);
	printf("sc = 0x%x ('%c')\n", (unsigned int)(unsigned char) sc, sc);
	printf("uc = 0x%x ('%c')\n", (unsigned int) uc, uc);
	printf("my+1[sc & _CTYPE_MASK] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(sc & _CTYPE_MASK)]);
	printf("my+1[(unsigned char)sc] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(unsigned char)(sc)]);
	printf("my+1[sc] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(sc)]);
	printf("isalpha(sc) = 0x%x %s\n", isalpha(sc), CORRECTP(!isalpha(sc)));
	printf("isalpha(uc) = 0x%x %s\n", isalpha(uc), CORRECTP(!isalpha(uc)));
	printf("my_isalpha(sc) = 0x%x %s\n", my_isalpha(sc), CORRECTP(!my_isalpha(sc)));
	printf("my_isalpha(uc) = 0x%x %s\n", my_isalpha(uc), CORRECTP(!my_isalpha(uc)));
	printf("na_isalpha(sc) = 0x%x %s\n", na_isalpha(sc), CORRECTP(!na_isalpha(sc)));
	printf("na_isalpha(uc) = 0x%x %s\n", na_isalpha(uc), CORRECTP(!na_isalpha(uc)));
	printf("na_isalpha(i) = 0x%x %s\n", na_isalpha(i), CORRECTP(!na_isalpha(i)));
	printf("islower(sc) = 0x%x %s\n", islower(sc), CORRECTP(!islower(sc)));
	printf("islower(uc) = 0x%x %s\n", islower(uc), CORRECTP(!islower(uc)));
	printf("isupper(sc) = 0x%x %s\n", isupper(sc), CORRECTP(!isupper(sc)));
	printf("isupper(uc) = 0x%x %s\n", isupper(uc), CORRECTP(!isupper(uc)));
	putchar('\n');


	/* XXX make sure we're (still) in the C locale */
	setlocale(LC_ALL, "C");
	my_ctype = my_ctype_ascii;
	my_tolower_tab = my_tolower_ascii;
	printf("setlocale(LC_ALL, \"C\");\n");
	sc = (signed char) 0xC1;	/* capital A, acute accent */
	uc = 0xC1;			/* capital A, acute accent */
	i = 0xC1;			/* capital A, acute accent */
	printf("0xC1, Capital A, acute accent: SC=%c\n", sc);
	printf("0xC1, Capital A, acute accent: UC=%c\n", uc);
	printf("sc = 0x%x ('%c')\n", (unsigned int)(unsigned char) sc, sc);
	printf("uc = 0x%x ('%c')\n", (unsigned int) uc, uc);
	printf("my+1[sc & _CTYPE_MASK] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(sc & _CTYPE_MASK)]);
	printf("my+1[(unsigned char)sc] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(unsigned char)(sc)]);
	printf("my+1[sc] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(sc)]);
	printf("isascii(sc) = 0x%x %s\n", isascii(sc), CORRECTP(!isascii(sc)));
	printf("isascii(uc) = 0x%x %s\n", isascii(uc), CORRECTP(!isascii(uc)));
	printf("isalpha(sc) = 0x%x %s\n", isalpha(sc), CORRECTP(!isalpha(sc))); /* xxx hmmmm.... why wrong??? */
	printf("isalpha(uc) = 0x%x %s\n", isalpha(uc), CORRECTP(!isalpha(uc)));
	printf("my_isalpha(sc) = 0x%x %s\n", my_isalpha(sc), CORRECTP(!my_isalpha(sc)));
	printf("my_isalpha(uc) = 0x%x %s\n", my_isalpha(uc), CORRECTP(!my_isalpha(uc)));
	printf("na_isalpha(sc) = 0x%x %s\n", na_isalpha(sc), CORRECTP(!na_isalpha(sc)));
	printf("na_isalpha(uc) = 0x%x %s\n", na_isalpha(uc), CORRECTP(!na_isalpha(uc)));
	printf("na_isalpha(i) = 0x%x %s\n", na_isalpha(i), CORRECTP(!na_isalpha(i)));
	printf("islower(sc) = 0x%x %s\n", islower(sc), CORRECTP(!islower(sc))); /* xxx hmmmm.... why wrong??? */
	printf("islower(uc) = 0x%x %s\n", islower(uc), CORRECTP(!islower(uc)));
	printf("isupper(sc) = 0x%x %s\n", isupper(sc), CORRECTP(!isupper(sc)));
	printf("isupper(uc) = 0x%x %s\n", isupper(uc), CORRECTP(!isupper(uc)));
	putchar('\n');

	printf("isasii(EOF) = 0x%x %s\n", isascii(EOF), CORRECTP(!isascii(EOF)));
	printf("isalpha(EOF) = 0x%x %s\n", isalpha(EOF), CORRECTP(!isalpha(EOF)));
	printf("my_isalpha(EOF) = 0x%x %s\n", my_isalpha(EOF), CORRECTP(!my_isalpha(EOF)));
	printf("islower(EOF) = 0x%x %s\n", islower(EOF), CORRECTP(!islower(EOF)));	/* negative int implicit convertion to unsigned */
	printf("isupper(EOF) = 0x%x %s\n", isupper(EOF), CORRECTP(!isupper(EOF)));	/* negative int implicit convertion to unsigned */
	printf("tolower(EOF) = 0x%x %s\n", tolower(EOF), CORRECTP(tolower(EOF) == EOF));
	printf("my_tolower(EOF) = 0x%x %s\n", my_tolower(EOF), CORRECTP(my_tolower(EOF) == EOF));
	putchar('\n');

	setlocale(LC_ALL, "en_US.ISO8859-1");
	my_ctype = my_ctype_8859_1;
	my_tolower_tab = my_tolower_8859_1;
	printf("setlocale(LC_ALL, \"en_US.ISO8859-1\");\n");
	sc = (signed char) 0xE1;	/* small a, acute accent */
	uc = 0xE1;			/* small a, acute accent */
	i = 0xE1;			/* small a, acute accent */
	printf("0xE1, small a, acute accent: SC=%c\n", sc);
	printf("0xE1, small a, acute accent: UC=%c\n", uc);
	printf("sc = 0x%x ('%c')\n", (unsigned int)(unsigned char) sc, sc);
	printf("uc = 0x%x ('%c')\n", (unsigned int) uc, uc);
	printf("my+1[sc & _CTYPE_MASK] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(sc & _CTYPE_MASK)]);
	printf("my+1[(unsigned char)sc] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(unsigned char)(sc)]);
	printf("my+1[sc] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(sc)]);
	printf("isalpha(sc) = 0x%x %s\n", isalpha(sc), CORRECTP(isalpha(sc)));
	printf("isalpha(uc) = 0x%x %s\n", isalpha(uc), CORRECTP(isalpha(uc)));
	printf("my_isalpha(sc) = 0x%x %s\n", my_isalpha(sc), CORRECTP(my_isalpha(sc)));
	printf("my_isalpha(uc) = 0x%x %s\n", my_isalpha(uc), CORRECTP(my_isalpha(uc)));
	printf("islower(sc) = 0x%x %s\n", islower(sc), CORRECTP(islower(sc)));
	printf("islower(uc) = 0x%x %s\n", islower(uc), CORRECTP(islower(uc)));
	printf("isupper(sc) = 0x%x %s\n", isupper(sc), CORRECTP(!isupper(sc))); /* xxx hmmmm.... why wrong??? */
	printf("isupper(uc) = 0x%x %s\n", isupper(uc), CORRECTP(!isupper(uc)));
	printf("tolower(sc) = 0x%x %s\n", tolower(sc), CORRECTP(tolower(sc) == 0xE1)); /* xxx hmmmm.... why 0???  sign extension? */
	printf("tolower(uc) = 0x%x %s\n", tolower(uc), CORRECTP(tolower(uc) == 0xE1));
	printf("my_tolower(sc) = 0x%x %s\n", my_tolower(sc), CORRECTP(my_tolower(sc) == 0xE1));
	printf("my_tolower(uc) = 0x%x %s\n", my_tolower(uc), CORRECTP(my_tolower(uc) == 0xE1));
	putchar('\n');

	setlocale(LC_ALL, "en_US.ISO8859-1");
	my_ctype = my_ctype_8859_1;
	my_tolower_tab = my_tolower_8859_1;
	printf("setlocale(LC_ALL, \"en_US.ISO8859-1\");\n");
	sc = (signed char) 0xC1;	/* capital A, acute accent */
	uc = 0xC1;			/* capital A, acute accent */
	i = 0xC1;			/* capital A, acute accent */
	printf("0xC1, Capital A, acute accent: SC=%c\n", sc);
	printf("0xC1, Capital A, acute accent: UC=%c\n", uc);
	printf("sc = 0x%x ('%c')\n", (unsigned int)(unsigned char) sc, sc);
	printf("uc = 0x%x ('%c')\n", (unsigned int) uc, uc);
	printf("my+1[sc & _CTYPE_MASK] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(sc & _CTYPE_MASK)]);
	printf("my+1[(unsigned char)sc] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(unsigned char)(sc)]);
	printf("my+1[sc] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(sc)]);
	printf("isalpha(sc) = 0x%x %s\n", isalpha(sc), CORRECTP(isalpha(sc)));
	printf("isalpha(uc) = 0x%x %s\n", isalpha(uc), CORRECTP(isalpha(uc)));
	printf("my_isalpha(sc) = 0x%x %s\n", my_isalpha(sc), CORRECTP(my_isalpha(sc)));
	printf("my_isalpha(uc) = 0x%x %s\n", my_isalpha(uc), CORRECTP(my_isalpha(uc)));
	printf("islower(sc) = 0x%x %s\n", islower(sc), CORRECTP(!islower(sc)));
	printf("islower(uc) = 0x%x %s\n", islower(uc), CORRECTP(!islower(uc)));
	printf("isupper(sc) = 0x%x %s\n", isupper(sc), CORRECTP(isupper(sc)));
	printf("isupper(uc) = 0x%x %s\n", isupper(uc), CORRECTP(isupper(uc)));
	printf("tolower(sc) = 0x%x %s\n", tolower(sc), CORRECTP(tolower(sc) == 0xE1));
	printf("tolower(uc) = 0x%x %s\n", tolower(uc), CORRECTP(tolower(uc) == 0xE1));
	printf("my_tolower(sc) = 0x%x %s\n", my_tolower(sc), CORRECTP(my_tolower(sc) == 0xE1));
	printf("my_tolower(uc) = 0x%x %s\n", my_tolower(uc), CORRECTP(my_tolower(uc) == 0xE1));
	putchar('\n');

	/* test a cast inside a macro call */ {
		char a[] = "aAbBcCdDeEfF";
		char *p = a;
		unsigned int j;

		for (j = 0; j < sizeof(a) - 1; j++) {
			if (MY_ISUPPER(p[j])) {
				p[j] = (char) MY_TOLOWER(p[j]);
			}
		}
		printf("tolower a[] = %s\n", a);
	}
	putchar('\n');

	/*  */ {
		const unsigned char a[] = "aAbBcCdDeEfF";
		t_in_cond(a, sizeof(a));
	}
	putchar('\n');

	/*
	 * XXX
	 *
	 * Here's where the problems occur:
	 *
	 * Since 0xFF is a valid 8-bit character value, the is*() and to*()
	 * _functions_ will be unable to distinguish the difference between
	 * this valid character and EOF when the 0xFF character is passed to
	 * them as a "signed char" (or possibly also a "char" if the compiler
	 * defaults to "-fsigned-char").
	 *
	 * Also, passing EOF to the is*() and to*() _functions_ using the "fix"
	 * would return not zero, but rather whatever is true for 0377 in the
	 * current locale.
	 */
	sc = (signed char) 0xFF;	/* small y, dieresis */
	uc = 0xFF;			/* small y, dieresis */
	i = 0xFF;			/* small y, dieresis */
	printf("0xFF, small y, dieresis: SC=%c\n", sc);
	printf("0xFF, small y, dieresis: UC=%c\n", uc);
	printf("sc = 0x%x ('%c')\n", (unsigned int)(unsigned char) sc, sc);
	printf("uc = 0x%x ('%c')\n", (unsigned int) uc, uc);
	printf("my+1[sc & _CTYPE_MASK] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(sc & _CTYPE_MASK)]);
	printf("my+1[(unsigned char)sc] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(unsigned char)(sc)]);
	printf("my+1[sc] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(sc)]);
	printf("isalpha(sc) = 0x%x %s\n", isalpha(sc), CORRECTP(isalpha(sc)));
	printf("isalpha(uc) = 0x%x %s\n", isalpha(uc), CORRECTP(isalpha(uc)));
	printf("isalpha(i) = 0x%x %s\n", isalpha(i), CORRECTP(isalpha(i)));
	printf("my_isalpha(sc) = 0x%x %s\n", my_isalpha(sc), CORRECTP(my_isalpha(sc)));
	printf("my_isalpha(uc) = 0x%x %s\n", my_isalpha(uc), CORRECTP(my_isalpha(uc)));
	printf("my_isalpha(i) = 0x%x %s\n", my_isalpha(i), CORRECTP(my_isalpha(i)));
	printf("MY_ISALPHA(sc) = 0x%x %s\n", MY_ISALPHA(sc), CORRECTP(MY_ISALPHA(sc)));
	printf("MY_ISALPHA(uc) = 0x%x %s\n", MY_ISALPHA(uc), CORRECTP(MY_ISALPHA(uc)));
	printf("MY_ISALPHA(i) = 0x%x %s\n", MY_ISALPHA(i), CORRECTP(MY_ISALPHA(i)));
	printf("islower(sc) = 0x%x %s\n", islower(sc), CORRECTP(islower(sc)));
	printf("islower(uc) = 0x%x %s\n", islower(uc), CORRECTP(islower(uc)));
	printf("islower(i) = 0x%x %s\n", islower(i), CORRECTP(islower(i)));
	printf("MY_ISLOWER(sc) = 0x%x %s\n", MY_ISLOWER(sc), CORRECTP(MY_ISLOWER(sc)));
	printf("MY_ISLOWER(uc) = 0x%x %s\n", MY_ISLOWER(uc), CORRECTP(MY_ISLOWER(uc)));
	printf("MY_ISLOWER(i) = 0x%x %s\n", MY_ISLOWER(i), CORRECTP(MY_ISLOWER(i)));
	printf("isupper(sc) = 0x%x %s\n", isupper(sc), CORRECTP(!isupper(sc)));
	printf("isupper(uc) = 0x%x %s\n", isupper(uc), CORRECTP(!isupper(uc)));
	printf("isupper(i) = 0x%x %s\n", isupper(i), CORRECTP(!isupper(i)));
	printf("MY_ISUPPER(sc) = 0x%x %s\n", MY_ISUPPER(sc), CORRECTP(!MY_ISUPPER(sc)));
	printf("MY_ISUPPER(uc) = 0x%x %s\n", MY_ISUPPER(uc), CORRECTP(!MY_ISUPPER(uc)));
	printf("MY_ISUPPER(i) = 0x%x %s\n", MY_ISUPPER(i), CORRECTP(!MY_ISUPPER(i)));
	printf("tolower(sc) = 0x%x %s\n", (unsigned int) tolower(sc), CORRECTP(tolower(sc) == 0xFF));
	printf("tolower(uc) = 0x%x %s\n", (unsigned int) tolower(uc), CORRECTP(tolower(uc) == 0xFF));
	printf("tolower(i) = 0x%x %s\n", (unsigned int) tolower(i), CORRECTP(tolower(i) == 0xFF));
	printf("my_tolower(sc) = 0x%x %s\n", my_tolower(sc), CORRECTP(my_tolower(sc) == 0xFF));
	printf("my_tolower(uc) = 0x%x %s\n", my_tolower(uc), CORRECTP(my_tolower(uc) == 0xFF));
	printf("my_tolower(i) = 0x%x %s\n", my_tolower(i), CORRECTP(my_tolower(i) == 0xFF));
	printf("MY_TOLOWER(sc) = 0x%x %s\n", (unsigned int) MY_TOLOWER(sc), CORRECTP(MY_TOLOWER(sc) == 0xFF));
	printf("MY_TOLOWER(uc) = 0x%x %s\n", (unsigned int) MY_TOLOWER(uc), CORRECTP(MY_TOLOWER(uc) == 0xFF));
	printf("MY_TOLOWER(i) = 0x%x %s\n", (unsigned int) MY_TOLOWER(i), CORRECTP(MY_TOLOWER(i) == 0xFF));
	putchar('\n');

	/* These work of coursen: */
	printf("isalpha((unsigned char) sc) = 0x%x %s\n", isalpha((unsigned char) sc), CORRECTP(isalpha((unsigned char) sc)));
	printf("my_isalpha((unsigned char) sc) = 0x%x %s\n", my_isalpha((unsigned char) sc), CORRECTP(my_isalpha((unsigned char) sc)));
	printf("islower((unsigned char) sc) = 0x%x %s\n", islower((unsigned char) sc), CORRECTP(islower((unsigned char) sc)));
	printf("isupper((unsigned char) sc) = 0x%x %s\n", isupper((unsigned char) sc), CORRECTP(!isupper((unsigned char) sc)));
	printf("tolower((unsigned char) sc) = 0x%x %s\n", tolower((unsigned char) sc), CORRECTP(tolower((unsigned char) sc) == 0xFF));
	printf("tolower((unsigned char) uc) = 0x%x %s\n", tolower((unsigned char) uc), CORRECTP(tolower((unsigned char) uc) == 0xFF));
	printf("my_tolower((unsigned char) sc) = 0x%x %s\n", my_tolower((unsigned char) sc), CORRECTP(my_tolower((unsigned char) sc) == 0xFF));
	printf("my_tolower((unsigned char) uc) = 0x%x %s\n", my_tolower((unsigned char) uc), CORRECTP(my_tolower((unsigned char) uc) == 0xFF));
	putchar('\n');

	sc = (signed char) 0xFE;	/* small thorn, Icelandic */
	uc = 0xFE;			/* small thorn, Icelandic */
	i = 0xFE;			/* small thorn, Icelandic */
	printf("0xFE, small thorn, Icelandic: SC=%c\n", sc);
	printf("0xFE, small thorn, Icelandic: UC=%c\n", uc);
	printf("sc = 0x%x ('%c')\n", (unsigned int)(unsigned char) sc, sc);
	printf("uc = 0x%x ('%c')\n", (unsigned int) uc, uc);
	printf("my+1[sc & _CTYPE_MASK] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(sc & _CTYPE_MASK)]);
	printf("my+1[(unsigned char)sc] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(unsigned char)(sc)]);
	printf("my+1[sc] = 0x%x\n",
	       (unsigned int)(my_ctype + 1)[(sc)]);
	printf("isalpha(sc) = 0x%x %s\n", isalpha(sc), CORRECTP(isalpha(sc)));
	printf("isalpha(uc) = 0x%x %s\n", isalpha(uc), CORRECTP(isalpha(uc)));
	printf("isalpha(i) = 0x%x %s\n", isalpha(i), CORRECTP(isalpha(i)));
	printf("my_isalpha(sc) = 0x%x %s\n", my_isalpha(sc), CORRECTP(my_isalpha(sc)));
	printf("my_isalpha(uc) = 0x%x %s\n", my_isalpha(uc), CORRECTP(my_isalpha(uc)));
	printf("my_isalpha(i) = 0x%x %s\n", my_isalpha(i), CORRECTP(my_isalpha(i)));
	printf("MY_ISALPHA(sc) = 0x%x %s\n", MY_ISALPHA(sc), CORRECTP(MY_ISALPHA(sc)));
	printf("MY_ISALPHA(uc) = 0x%x %s\n", MY_ISALPHA(uc), CORRECTP(MY_ISALPHA(uc)));
	printf("MY_ISALPHA(i) = 0x%x %s\n", MY_ISALPHA(i), CORRECTP(MY_ISALPHA(i)));
	printf("islower(sc) = 0x%x %s\n", islower(sc), CORRECTP(islower(sc)));
	printf("islower(uc) = 0x%x %s\n", islower(uc), CORRECTP(islower(uc)));
	printf("islower(i) = 0x%x %s\n", islower(i), CORRECTP(islower(i)));
	printf("MY_ISLOWER(sc) = 0x%x %s\n", MY_ISLOWER(sc), CORRECTP(MY_ISLOWER(sc)));
	printf("MY_ISLOWER(uc) = 0x%x %s\n", MY_ISLOWER(uc), CORRECTP(MY_ISLOWER(uc)));
	printf("MY_ISLOWER(i) = 0x%x %s\n", MY_ISLOWER(i), CORRECTP(MY_ISLOWER(i)));
	printf("isupper(sc) = 0x%x %s\n", isupper(sc), CORRECTP(!isupper(sc)));
	printf("isupper(uc) = 0x%x %s\n", isupper(uc), CORRECTP(!isupper(uc)));
	printf("isupper(i) = 0x%x %s\n", isupper(i), CORRECTP(!isupper(i)));
	printf("MY_ISUPPER(sc) = 0x%x %s\n", MY_ISUPPER(sc), CORRECTP(!MY_ISUPPER(sc)));
	printf("MY_ISUPPER(uc) = 0x%x %s\n", MY_ISUPPER(uc), CORRECTP(!MY_ISUPPER(uc)));
	printf("MY_ISUPPER(i) = 0x%x %s\n", MY_ISUPPER(i), CORRECTP(!MY_ISUPPER(i)));
	printf("tolower(sc) = 0x%x %s\n", (unsigned int) tolower(sc), CORRECTP(tolower(sc) == 0xFE));
	printf("tolower(uc) = 0x%x %s\n", (unsigned int) tolower(uc), CORRECTP(tolower(uc) == 0xFE));
	printf("tolower(i) = 0x%x %s\n", (unsigned int) tolower(i), CORRECTP(tolower(i) == 0xFE));
	printf("my_tolower(sc) = 0x%x %s\n", my_tolower(sc), CORRECTP(my_tolower(sc) == 0xFE));
	printf("my_tolower(uc) = 0x%x %s\n", my_tolower(uc), CORRECTP(my_tolower(uc) == 0xFE));
	printf("my_tolower(i) = 0x%x %s\n", my_tolower(i), CORRECTP(my_tolower(i) == 0xFE));
	printf("MY_TOLOWER(sc) = 0x%x %s\n", (unsigned int) MY_TOLOWER(sc), CORRECTP(MY_TOLOWER(sc) == 0xFE));
	printf("MY_TOLOWER(uc) = 0x%x %s\n", (unsigned int) MY_TOLOWER(uc), CORRECTP(MY_TOLOWER(uc) == 0xFE));
	printf("MY_TOLOWER(i) = 0x%x %s\n", (unsigned int) MY_TOLOWER(i), CORRECTP(MY_TOLOWER(i) == 0xFE));
	putchar('\n');

	printf("isalpha(EOF) = 0x%x %s\n", isalpha(EOF), CORRECTP(!isalpha(EOF)));
	printf("my_isalpha(EOF) = 0x%x %s\n", my_isalpha(EOF), CORRECTP(!my_isalpha(EOF)));
	printf("MY_ISALPHA(EOF) = 0x%x %s\n", MY_ISALPHA(EOF), CORRECTP(!MY_ISALPHA(EOF)));
	printf("na_isalpha(EOF) = 0x%x %s\n", na_isalpha(EOF), CORRECTP(!na_isalpha(EOF)));
	printf("islower(EOF) = 0x%x %s\n", islower(EOF), CORRECTP(!islower(EOF)));
	printf("isupper(EOF) = 0x%x %s\n", isupper(EOF), CORRECTP(!isupper(EOF)));
	printf("tolower(EOF) = 0x%x %s\n", (unsigned int) tolower(EOF), CORRECTP(tolower(EOF) == EOF));
	printf("my_tolower(EOF) = 0x%x %s\n", my_tolower(EOF), CORRECTP(my_tolower(EOF) == EOF));
	printf("MY_TOLOWER(EOF) = 0x%x %s\n", (unsigned int) MY_TOLOWER(EOF), CORRECTP(MY_TOLOWER(EOF) == EOF));
	printf("na_tolower(EOF) = 0x%x %s\n", (unsigned int) na_tolower(EOF), CORRECTP(na_tolower(EOF) == EOF));
	putchar('\n');


	printf("islower(): -1=0x%x, 255=0x%x, (unsigned char)-1=0x%x, -1&0xFF=0x%x\n",
	       islower(-1),		/* negative int implicit convertion to unsigned */
	       islower(255),
	       islower((unsigned char)-1),
	       islower(-1 & 0xFF));

	printf("isprint(): 0=0x%x, 1=0x%x, a=0x%x, -1=0x%x, 255=0x%x, (unsigned char)-1=0x%x, -1&0xFF=0x%x\n",
	       isprint(0),
	       isprint(1),
	       isprint((int) 'a'),
	       isprint(-1),		/* negative int implicit convertion to unsigned */
	       isprint(255),
	       isprint((unsigned char)-1),
	       isprint(-1 & 0xFF));

	test_skipwhitespace("blah");
	test_skipwhitespace(" one leading");
	test_skipwhitespace("  two leading");
	test_skipwhitespace("\ttab leading");
	test_skipwhitespace(" \tspace+tab leading");
	test_skipwhitespace("");

	puts("\nNOTICE:  these may not skip whitespace with some compilers:");

	test_skipwhitespace_broken("blah");
	test_skipwhitespace_broken(" one leading");
	test_skipwhitespace_broken("  two leading");
	test_skipwhitespace_broken("\ttab leading");
	test_skipwhitespace_broken(" \tspace+tab leading");
	test_skipwhitespace_broken("");

#if defined(__NetBSD__) && !defined(_CTYPE_A)

	/* XXX to undo my local NetBSD mods! */
#undef	isdigit
#define	isdigit(c)	((int)((_ctype_ + 1)[(c)] & _N))
#undef	islower
#define	islower(c)	((int)((_ctype_ + 1)[(c)] & _L))
#undef	isspace
#define	isspace(c)	((int)((_ctype_ + 1)[(c)] & _S))
#undef	ispunct
#define	ispunct(c)	((int)((_ctype_ + 1)[(c)] & _P))
#undef	isupper
#define	isupper(c)	((int)((_ctype_ + 1)[(c)] & _U))
#undef	isalpha
#define	isalpha(c)	((int)((_ctype_ + 1)[(c)] & (_U|_L)))
#undef	isxdigit
#define	isxdigit(c)	((int)((_ctype_ + 1)[(c)] & (_N|_X)))
#undef	isalnum
#define	isalnum(c)	((int)((_ctype_ + 1)[(c)] & (_U|_L|_N)))
#undef	isprint
#define	isprint(c)	((int)((_ctype_ + 1)[(c)] & (_P|_U|_L|_N|_B)))
#undef	isgraph
#define	isgraph(c)	((int)((_ctype_ + 1)[(c)] & (_P|_U|_L|_N)))
#undef	iscntrl
#define	iscntrl(c)	((int)((_ctype_ + 1)[(c)] & _C))
#undef	tolower
#define	tolower(c)	((int)((_tolower_tab_ + 1)[(c)]))
#undef	toupper
#define	toupper(c)	((int)((_toupper_tab_ + 1)[(c)]))

	(void) isalpha(i);
	(void) isalpha(l);
	(void) isalpha(c);		/* array subscript has type 'char' */
	(void) isalpha(ui);
	(void) isalpha(ul);
	(void) isalpha(uc);
	(void) isalpha(sc);
	(void) tolower(i);
	(void) tolower(l);
	(void) tolower(c);		/* array subscript has type 'char' */
	(void) tolower(ui);
	(void) tolower(ul);
	(void) tolower(uc);
	(void) tolower(sc);

	(void) isalpha(EOF);		/* XXX no warning! */
	(void) _tolower(EOF);
	(void) tolower(EOF);		/* XXX no warning! */

#endif

	printf("\nstrcasecmp('files', 'FILES') = %d %s\n",
	       my_strcasecmp("files", "FILES"),
	       CORRECTP(my_strcasecmp("files", "FILES") == 0));

	exit(0);
	/* NOTREACHED */
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " CPPFLAGS='-std=c89 -Wno-sign-compare' " fn " && ./" fn))
 * End:
 */
