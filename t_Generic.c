#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

/* messing with C11 _Generic() */

/* see also:  http://www.robertgamble.net/2012/01/c11-generic-selections.html */

#define typename(x) _Generic((x),        /* Get the name of a type */             \
                                                                                  \
        _Bool: "_Bool",                  unsigned char: "unsigned char",          \
         char: "char",                     signed char: "signed char",            \
    short int: "short int",         unsigned short int: "unsigned short int",     \
          int: "int",                     unsigned int: "unsigned int",           \
     long int: "long int",           unsigned long int: "unsigned long int",      \
long long int: "long long int", unsigned long long int: "unsigned long long int", \
        float: "float",                         double: "double",                 \
  long double: "long double",                   char *: "pointer to char",        \
       void *: "pointer to void",                int *: "pointer to int",         \
      default: "other")

#define fmt "%25s is '%s'\n"

int main(void);

int
main()
{
	size_t s;
	ptrdiff_t p;
	intmax_t i;
	int ai[3] = {0};
	bool b = false;
	const char *cs = "foobarnone";

	printf(fmt fmt fmt fmt fmt fmt fmt fmt fmt fmt fmt,
                   "size_t", typename(s),
                "ptrdiff_t", typename(p),
                 "intmax_t", typename(i),
       "character constant", typename('0'),
"character string constant", typename("foobar"), /* xxx why isn't this 'const'-qualified!?!?!? */
               "0x7FFFFFFF", typename(0x7FFFFFFF),
               "0xFFFFFFFF", typename(0xFFFFFFFF),
              "0x7FFFFFFFU", typename(0x7FFFFFFFU),
             "array of int", typename(ai),
	             "bool", typename(b),
        "const char string", typename(cs)); /* xxx qualifiers mess us up badly! */

	exit(EXIT_SUCCESS);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " CSTD='c11' " fn " && ./" fn))
 * End:
 */
