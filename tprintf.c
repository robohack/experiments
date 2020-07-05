#include <inttypes.h>
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

	printf("%.5s\n", (char []){'h', 'e', 'l', 'l', 'o' });

	sprintf(pbuf, "\r\nconsole=%3s%1d[0x%04X] @%06d baud\r\n",
		"com",
		0,
		0x2E8,
		38400);
	printf("string = '%s', len=%d\n", pbuf, (int) strlen(pbuf)); /* %d vs. size_t only noticed on I32LP64 */
	for (p = &pbuf[0]; *p; p++)
		putchar(*p);

	putchar('\n');

	printf("this is all escaped: \b\a\c\k\s\l\a\s\h\e\d\.\:\$\@\&\\\-\=\;\.\n");
	putchar('\n');

	/*
	 * `  aa'    `  bb'    `cc  '
	 */
	printf("`%4s'    `% 4s'    `%-4s'\n", "aa", "bb", "cc");
	printf("`%*s'    `% *s'    `%-*s'\n", 4, "aa", 4, "bb", 4, "cc");
	printf("`%*s'    `% *s'    `%-*s'\n", 4, "aaaaXXX", 4, "bbbbXXX", 4, "ccccXXX");
	printf("`%*.*s'    `% *.*s'    `%-*.*s'\n", 2, 4, "aaaaXXX", 2, 4, "bbbbXXX", 2, 4, "ccccXXX");
	printf("`%*.*s'    `% *.*s'    `%-*.*s'\n", 4, 2, "aaaaXXX", 4, 2, "bbbbXXX", 4, 2, "ccccXXX");
	printf("`%4.*s'    `% 4.*s'    `%-4.*s'\n", 2, "aaaaXXX", 2, "bbbbXXX", 2, "ccccXXX");
	printf("`%.*s'    `% .*s'    `%-.*s'\n", 4, "aaaaXXX", 4, "bbbbXXX", 4, "ccccXXX");
	putchar('\n');

	/*
	 *   hex    octal  decimal
	 * >  7b<   > 173<   > 123<
	 * >007b<   >0173<   >0123<
	 *
	 * Note that GCC -Wformat does not like the '#' (alternate form), only
	 * has effect for 'o', 'x', 'X', and "aAeEfFgG")
	 */
	printf("  >%4x<   >%4o<   >%4d<\n", 123, 123, 123);
	printf("  >%4.4x<   >%4.4o<   >%4.4d<\n", 123, 123, 123);
	printf(">%4.4#x<   >%4.4#o<   >%4.4#d<\n", 123, 123, 123);
	printf(">%*.*#x<   >%*.*#o<   >%*.*#d<\n", 4, 4, 123, 4, 4, 123, 4, 4, 123);
	printf("  >%4#x<   >%4#o<   >%4#d<\n", 123, 123, 123); /* see the "0x"(x) and "0"(o) */

	putchar('\n');

	printf("  |%0*x|   |%0*o|   |%0*d|\n", 8, 123, 8, 123, 8, 123);
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

	exit(0);
}


/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
