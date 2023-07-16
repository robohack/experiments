#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

/*
 *	Demonstrate the problems of naive trunction of the getchar() result.
 *
 * leaving this defined demonstrates the problem of char as unsigned, unless one
 * of the USE_* fixes is uncommented below.
 *
 * If this is commented out then the HALF_BAKED_SOLUTION will be used (unless it
 * is also commented out), demonstrating how code can appear to "work" when char
 * is signed by default, though it will still be technically wrong!  Note that
 * for many Unix users on signed char platforms this remaining problem was still
 * invisible to them because the TTY driver treated RUBOUT (0xFF) as the
 * interrupt character, making it a de facto impossible result from getchar().
 */
#define IGNORE_EOF			/* demo problem if char is unsigned */

#ifdef IGNORE_EOF
/*
 * uncomment one of the following to deal with "unsigned char" comparison to EOF
 *
 * N.B.:  these options still lose -- they hide the real value of 0xFF!
 */
//# define USE_CASTING_SIGN_EXTENSION	/* catch EOF with sign extenstion! */
//# define USE_BITFIELDS  		/* catch EOF with sign extenstion! */
#endif

#ifndef IGNORE_EOF
/*
 * Commenting this out gives the correct solution!
 */
# define HALF_BAKED_SOLUTION	/* the reason some naive code works */
#endif

#ifdef IGNORE_EOF
typedef unsigned char ch_t;	/* explicit "unsigned char" */
#else
# ifdef HALF_BAKED_SOLUTION
typedef signed char ch_t;	/* explicit "signed char" */
# else
typedef int ch_t;		/* the only correct solution: leave it alone! */
# endif
#endif

/*
 * Note that it is also possible to sign-extend the truncated value using struct
 * bit fields!
 *
 * See: https://graphics.stanford.edu/~seander/bithacks.html#FixedSignExtend
 */
struct {
	signed int x:CHAR_BIT;
} s;

enum {
	SZ = 20,
};

int
main(void)
{
	int i = 0;
	ch_t ch;
	unsigned char array[SZ + 1];

	if (CHAR_MIN < 0) {
		printf("NOTE:  plain 'char' is signed by default!\n\n");
	}

	/*
	 * getchar(3) returns a char value, or EOF, thus it is declared as
	 * returning (signed) int.  As a result it is critical to do things in
	 * the right order to avoid truncating an EOF value to 0xFF and thus
	 * making any comparison with EOF be implicitly always different.
	 */
#ifdef USE_BITFIELDS
	/*
	 * Using bitfields to force sign extension is a bit over-the-top (as
	 * we'll see in the next example), but it works:
	 *
	 * N.B.:  However this makes it impossible to read a real 0xFF
	 * character, as a real 0xFF will compare equally to EOF and be lost.
	 */
	while (ch = (unsigned char) getchar(), s.x = (signed char) ch, s.x != EOF && i < SZ)
#else
# ifdef USE_CASTING_SIGN_EXTENSION
	/*
	 * If we just explictily cast the unsigned variable to "signed" (as is
	 * also done in the bitfields example) then this conversion happens
	 * before integer promotion and so when the result of the cast is
	 * compared with EOF (-1) the default integer promotion extends the sign
	 * as expected and the comparison works as expected, i.e. "preserving"
	 * the value of EOF through conversion to an unsigned char.
	 *
	 * I.e. it is safer to use the comma operator to separate the assignment
	 * and comparison than to combine everything and compare the result of
	 * the assignment!
	 *
	 * N.B.:  However this makes it impossible to read a real 0xFF
	 * character, as a real 0xFF will compare equally to EOF and be lost.
	 */
	while (ch = (unsigned char) getchar(), (signed char) ch != EOF && i < SZ)
# else /* this naive form of the expression only works with signed char... */
	/*
	 * n.b.:  here if the result of getchar() is first cast to an (unsigned
	 * char) the cast truncates the value from -1 to 0xFFU, but a subsequent
	 * (int) cast of that result (or an implicit promotion to int) won't
	 * sign-extend it because it's coming from an unsigned value.
	 *
	 * So, if "ch" is declared plain "char", but the platform's "char" is
	 * actually "unsigned char", then we have a problem (with the default
	 * while condition)!
	 *
	 * Most compilers these days will give a clue to this problem by
	 * complaining that the "!= EOF" comparison is always true due to the
	 * limited range of (one of) the data types involved.
	 *
	 * N.B.:  Unless ch_t is int this still makes it impossible to read a
	 * real 0xFF character!
	 */
	while (((int) (ch = (ch_t) getchar()) != EOF) && i < SZ)
# endif
#endif
	{
		if (ch == '\n' || i >= SZ) {
			array[i] = '\0';
			printf("%s\n", array);
			i = 0;
		} else if ((((ch + 1) & ch) == 0) && ch != 0) { /* all 1's test without xor */
			printf("adding 0xFF to the array...\n");
		}
		array[i] = (unsigned char) ch;
		i += 1;
	}
	array[i] = '\0';

	printf("%s\n", array);

	exit(0);
}


/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " " fn " && echo foo bar none | ./" fn " | cat -v"))
 * End:
 */
