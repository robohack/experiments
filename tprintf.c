#include <inttypes.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int, char **);

int
main(argc, argv)
	int argc;
	char *argv[] __unused;
{
	static char pbuf[BUFSIZ];
	char *p;
	time_t t = -1;
	int32_t i32val = -1;
	int64_t i64val = -1;

	printf("%.5s\n", (char []){'h', 'e', 'l', 'l', 'o' }); /* xxx ISO C90 forbids compound literals (without --std=c99) */

	sprintf(pbuf, "\r\nconsole=%3s%1d[0x%04X] @%06d baud\r\n",
		"com",
		0,
		0x2E8,
		38400);
	printf("string = '%s', len=%d\n", pbuf, (int) strlen(pbuf)); /* %d vs. size_t only noticed on I32LP64 */
	for (p = &pbuf[0]; *p; p++)
		putchar(*p);

	putchar('\n');

	printf("this is all escaped: \b\a\c\k\s\l\a\s\h\e\d\.\:\$\@\&\\\-\=\;\.\n"); /* xxx unknown escape sequence(x16) non-ISO-standard escape sequence, '\e' */
	putchar('\n');

	/*
	 * `  aa'    `  bb'    `cc  '
	 */
	/* xxx GCC (with -Wformat=2) doesn't like the ' ' flag.... */
	printf("`%4s'    `% 4s'    `%-4s'\n", "aa", "bb", "cc");
	printf("`%*s'    `% *s'    `%-*s'\n", 4, "aa", 4, "bb", 4, "cc");
	printf("`%*s' `% *s' `%-*s'\n", 4, "aaaaXXX", 4, "bbbbXXX", 4, "ccccXXX");
	printf("`%*.*s'    `% *.*s'    `%-*.*s'\n", 2, 4, "aaaaXXX", 2, 4, "bbbbXXX", 2, 4, "ccccXXX");
	printf("`%*.*s'    `% *.*s'    `%-*.*s'\n", 4, 2, "aaaaXXX", 4, 2, "bbbbXXX", 4, 2, "ccccXXX");
	printf("`%4.*s'    `% 4.*s'    `%-4.*s'\n", 2, "aaaaXXX", 2, "bbbbXXX", 2, "ccccXXX");
	printf("`%.*s'    `% .*s'    `%-.*s'\n", 4, "aaaaXXX", 4, "bbbbXXX", 4, "ccccXXX");
	printf("`%.*s'      `% .*s'      `%-.*s'\n", 4, "aa", 4, "bb", 4, "cc");
	putchar('\n');

	/*
	 *   hex    octal  decimal
	 * >  7b<   > 173<   > 123<
	 * >007b<   >0173<   >0123<
	 */
	printf("  >%4x<   >%4o<   >%4d<\n", 123, 123, 123);
	printf("  >%4.4x<   >%4.4o<   >%4.4d<\n", 123, 123, 123);
	printf("  >%04.4x<   >%04.4o<   >%04.4d<\n", 123, 123, 123); /* warning: '0' flag ignored with precision and ‘%x’ gnu_printf format [-Wformat=] */
	printf("  >%.4x<   >%.4o<   >%.4d<\n", 123, 123, 123);
	/*
	 * N.B.:  GCC -Wformat does not like the '#' (alternate form), only has
	 * effect for 'o', 'x', 'X', and "aAeEfFgG")
	 */
	printf(">%4.4#x<   >%4.4#o<   >%4.4#d<\n", 123, 123, 123);
	printf(">%*.*#x<   >%*.*#o<   >%*.*#d<\n", 4, 4, 123, 4, 4, 123, 4, 4, 123);
	printf("  >%4#x<   >%4#o<   >%4#d<\n", 123, 123, 123); /* see the "0x"(x) and "0"(o) */

	putchar('\n');
	/*
	 *     In no case does a non-existent or small field width cause truncation of a
	 *     numeric field; if the result of a conversion is wider than the field
	 *     width, the field is expanded to contain the conversion result.
	 */
	printf("  >%02.2x<   >%02.2o<   >%02.2d<\n", 123, 123, 123);

	putchar('\n');

	printf("  |%0*x|   |%0*o|   |%0*d|\n", 8, 123, 8, 123, 8, 123);
	/*
	 * N.B.:  GCC -Wformat does not like the '#' (alternate form), only has
	 * effect for 'o', 'x', 'X', and "aAeEfFgG")
	 */
	printf("  |%0*#x|   |%0*#o|   |%0*#d|\n", 8, 123, 8, 123, 8, 123);
	printf("|%0*.*#x|   |%0*.*#o|   |%0*.*#d|\n", 8, 8, 123, 8, 8, 123, 8, 8, 123);

	putchar('\n');

	{
		static double dtmp;

		dtmp = 110.38214649949016;
		printf("%4.1f\n", dtmp);
		printf("%4.5f\n", 111.38214649949016);
		printf("\n");
		printf(" >%4.1f<   >%4.1e<   >%4.1g<\n", (double) 123, (double) 123, (double) 123);
		printf("\n");
	}

	printf("pointer as %%#.%dlx  = %#.*lx\n", (int) sizeof(long) * 2, (int) sizeof(long) * 2, (long) &argc);
	printf("pointer using %%p   = %p\n", (void *) &argc);
	printf("pointer using %%#p  = %#p\n", (void *) &argc); /* # with %p */
	printf("pointer using %%0p  = %0p\n", (void *) &argc); /* 0 with %p */
	printf("pointer using %%.0p = %.0p\n", (void *) &argc); /* precision with %p */
	printf("pointer using %%0.0p= %0.0p\n", (void *) &argc); /* 0 and precision with %p */
	printf("pointer with %%#.*p = %#.*p\n", (int) sizeof(void *) * 2, (void *) &argc); /* # and precision with %p */

	printf("%" PRId32 "\n", i32val);	/* ok.... */
	printf("%" PRId64 "\n", i64val);	/* ok.... */

	printf("%" PRId32 "\n", (int) -1);	/* ok.... */
	printf("%" PRId32 "\n", (long) -1); /* expects int, has long */
	printf("%" PRId32 "\n", (time_t) -1); /* expects int, has long !!! */
	printf("%" PRId32 "\n", t);	/* expects int, has time_t */
	printf("%" PRId32 "\n", (int32_t) t); /* OK!!!! */

	printf("%" PRIdMAX "\n", (intmax_t) t); /* OK!!! */

	printf("%jd\n", (intmax_t) t); /* OK!!! */
	printf("%ju\n", (uintmax_t) t); /* OK!!! */

	printf("%jd\n", (int64_t) t);	/* XXX OK on NetBSD/amd64 */
	printf("%ju\n", (uint64_t) t);	/* XXX OK on NetBSD/amd64 */

	/* POSIX 2008 print with thousand's separator */ {
		struct lconv *lc;

#ifdef __NetBSD__
		/*
		 * XXX NetBSD's "native" environment is probably "C" or "POSIX",
		 * not something user friendly.  Note also there's no plain
		 * "en", nor "en_CA", nor "en_US" -- there must be a teritory
		 * name, and the codset name must also be appended!  (Though
		 * there is an "en@boldquot" and "en@quot".)
		 */
		setlocale(LC_ALL, "en_CA.UTF-8"); /* esp. LC_MONETARY and LC_NUMERIC */
#else
		setlocale(LC_ALL, "");
#endif
		lc = localeconv();
		/*
		 * N.B.: .mon_grouping and .grouping are:
		 *
		 *     "a pointer to a vector of integers, each of size char,
		 *      representing group size from low order digit groups to
		 *      high order (right to left).  The list may be terminated
		 *      with 0 or CHAR_MAX.  If the list is terminated with 0,
		 *      the last group size before the 0 is repeated to account
		 *      for all the digits.  If the list is terminated with
		 *      CHAR_MAX, no more grouping is performed."
		 */
		printf("Printing currency of '%s', with '%s' group separator for monitary thousands @%s: %'d\n",
		       lc->currency_symbol,
		       lc->mon_thousands_sep,
		       lc->mon_grouping,
		       1234567890);
		printf("Printing currency of '%s', with '%s' group separator for monitary thousands @%s: %'f\n",
		       lc->currency_symbol,
		       lc->mon_thousands_sep,
		       lc->mon_grouping,
		       1234567890.99);
		printf("Printing with '%s' group separator for thousands @%s: %'d\n",
		       lc->thousands_sep,
		       lc->grouping,
		       1234567890);
		printf("Printing with '%s' group separator for thousands @%s: %'f\n",
		       lc->thousands_sep,
		       lc->grouping,
		       1234567890.99);
	}
	exit(0);
}


/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " CSTD=c99 " fn " && ./" fn))
 * End:
 */
