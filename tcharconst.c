#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

typedef uint32_t my_wchar_t;

/*
 * examine the assembler output to see how these are managed
 */
#if __GNUC_PREREQ__(4, 4)		/* xxx #ifdef __CHAR32_TYPE__ */

# if ((__STDC_VERSION__ - 0) >= 199901L)	/* i.e. supports Universal Character Names */

# ifndef __STRICT_ANSI__
typeof(U'x') foo_U = U'☎';
# endif
unsigned long sizeof_U = sizeof(U'☎');

#  ifndef __STRICT_ANSI__
typeof(L'\u20AC') foo_L = L'\u20AC';
#  endif
unsigned long sizeof_L = sizeof(L'\u20AC');

#  ifndef __STRICT_ANSI__
typeof(L'\U0001F6F8') foo_Lw = L'\U0001F6F8';
#  endif
unsigned long sizeof_Lw = sizeof(L'\U0001F6F8');

# endif  /* c99 */

#elif __GNUC_PREREQ__(3, 4)		/* xxx #ifdef __WCHAR_TYPE__ */

# ifndef __STRICT_ANSI__
typeof(L'x') foo_L = L'☎';
# endif
unsigned long sizeof_L = sizeof(L'☎');

#else /* GCC < 3.4  */

/*
 * no cigar... err, black telephone
 *
 * C89 allows wide character constants like L'x', but will barf on, or do the
 * wrong thing with, UTF-8 encoding where three or more bytes are needed for a
 * character.
 */
wchar_t wc = L'x';
unsigned long sizeof_wchar = sizeof(L'x');

#endif


int
main(void)
{
	signed int si = 0x7fffffff;
	unsigned int ui = 0xffffffff;

	unsigned short short_var = 0xffff;
	unsigned char char_var = 0xff;

	/*
	 * UTF-8 "wide" character constants
	 *
	 * e.g.:  '€'	8364, #o20254, #x20ac (UTF-8: #xE2 #x82 #xAC)
	 * or:    '☎'	9742, #o23016, #x260e (UTF-8: #xE2 #x98 #x8E)
	 *
	 * too bad ☎ wasn't \u0A28 (2600 base 10), or even \u2600
	 */
	/*
	 * Note none of this works for decomposed characters of course, i.e. a
	 * character plus one or more "combining marks", since those are multiple
	 * code points.
	 *
	 * Conversely if any pre-composed characters are used for a constant then
	 * they won't compare to code points from a string which has the
	 * decomposed character and combining mark (unless the string is first
	 * "normalized" to use the pre-composed form, e.g. using NFC or NFKC
	 * normalization).  (perhaps https://julialang.org/utf8proc/)
	 */
#if __GNUC_PREREQ__(4, 4)		/* xxx #ifdef __CHAR32_TYPE__ */

	my_wchar_t c8 = U'☎';
# define WC(x)	U##x	/* U'x' has the type __CHAR32_TYPE__ (char32_t) */

#elif __GNUC_PREREQ__(3, 4)		/* xxx #ifdef __WCHAR_TYPE__ */

	my_wchar_t c8 = L'☎';
# define WC(x)	L##x	/* L'x' has the type __WCHAR_TYPE__ (wchar_t) */

#else /* GCC < 3.4  */
	/* no cigar... err, black telephone */
#endif

	unsigned char c1 = 'a';

#ifdef WC
	my_wchar_t c8w = WC('☎');
	my_wchar_t c8ucn = WC('\u260E');

# if  __GNUC_PREREQ__(4, 4)
	my_wchar_t c9w = WC('\U0001F6F8');	/* FLYING SAUCER */

	assert(c9w == 0x1F6F8);
# endif
	assert(c1 == WC('a'));
	assert(c8 == c8ucn);
	assert(c8 == 0x260e);
	assert(c8w == c8);
	switch(c8ucn) {
	case WC('☎'):
		break;
	default:
		assert(false);
		break;
      }
#endif	/* WC */

	exit(0);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat (default-value 'compile-command) " CPPFLAGS='-std=gnu99' " fn " && ./" fn))
 * End:
 */
