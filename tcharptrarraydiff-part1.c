#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "tcharptrarraydiff.h"

const char *nut = "This is the first string";
const char nuts[] = "This is is an array of chars";

int a = 3;

int main(void);

int
main()
{
	union {
		long long unsigned int	iv;
		char *pv;
	} foo_as_ptr = { 0 };
	unsigned int i;
	unsigned char digits[] = "123456789";
	int a;				/* xxx shadow's global 'a' of same type */

	printf("a = %d\n", a);		/* prints garbage.... */ /* xxx warning: 'a' is used uninitialized in this function */
	/*
	 * foo is actually defined in part2 as an array of characters and is
	 * initialized with a string starting with "12345678"
	 *
	 * Here in part1 we've declared it as a pointer to a char, and since
	 * the pointer refers to the same memory object, it will appear to have
	 * been initialized with the byte values from the beginning of the
	 * string.
	 *
	 * XXX is this really endian-independent?
	 *
	 * Also note the cast of the (unsigned char) from digits is necessary
	 * because stupid Plauger forced ANSI to make C preserve values instead
	 * of preserving signedness, so unsigned char will still promote to
	 * _signed_ int, and could only promote to unsigned int IFF it could
	 * contain values not representable in the signed promoted type.  GCC's
	 * -Wconversion option doesn't warn about this until maybe as late as
	 * 4.7.x (4.2.1 does not warn, and Reehr suggests 4.4 may not warn in
	 * at least some cases for some types).
	 *
	 * (see: <url:http://blog.regehr.org/archives/268>)
	 *
	 * (see also: <http://www.virjacode.com/tutorials/c_int_promotion/>)
	 */
	for (i = 0; i < sizeof(foo_as_ptr.pv); i++) {
		foo_as_ptr.iv |= ((long long unsigned int) digits[i] << (i * CHAR_BIT));
	}
	printf("You will see that foo will have the value: %p\n", foo_as_ptr.pv);

	putchar('\n');

	printf("digits = %p [%s]\n", digits, digits);
	printf("nuts = %p\n", nuts);
	printf("foo = %p\n", foo);
	printf("ok = %p\n", ok);
	printf("str = %p\n", str);
	printf("strarr = %p\n", strarr);
	printf("szarr = %p\n", szarr);

	putchar('\n');

	/*
	 * str and strarr are both defined in part2 as char * and they are
	 * initialized to point to separate constant strings
	 *
	 * Here in part1 strarr is declared as an array of chars of unknown
	 * size.
	 *
	 * once again, since they both refer to the same memory object, it will
	 * appear here as a string (array of chars) that has been initialized
	 * with the byte values of the pointer, i.e. the address of the
	 * constant string defined in part2.
	 */

	bar2("foo", foo);
	bar2("&foo", &foo);
	bar2("str", str);
	bar2("strarr", strarr);
	bar2("&strarr", &strarr);

	bar2("&szarr", &szarr);
	bar2("&szarr", &szarr);

	printf("You will see here that char strarr[] contains the bytes of the address of\n"
	       "  part2's char *strarr pointer (in the natural machine order):\n");
	for (i = 0; i < sizeof(char *); i++) {
		printf("strarr[%u] = %02x\n", i, (unsigned) strarr[i] & 0xff);
	}
	putchar('\n');

	bar1(strarr);			/* XXX will print garbage, the bytes of strarr */

	bar1(foo);			/* XXX will cause a core dump */

	exit(0);
}

void
bar1(char *f)
{
	printf("%s\n", f);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm tcharptrarraydiff tcharptrarraydiff-part*.o; " (default-value 'compile-command) " OPTIM=-O0 PROG=tcharptrarraydiff SRCS='tcharptrarraydiff-part1.c tcharptrarraydiff-part2.c' && ./tcharptrarraydiff"))
 * End:
 */
