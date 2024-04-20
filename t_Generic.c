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
 const char *: "const pointer to char",                                           \
      default: "other")

#define fmt "%25s is '%s'\n"

int main(void);

int
main()
{
	size_t s __unused;
	ptrdiff_t p __unused;
	intmax_t i __unused;
	int ai[3] __unused = {0};
	int *pi __unused;
	bool b __unused = false;
	char c __unused = 'c';
	signed char sc __unused = 's';
	unsigned char uc __unused = 'u';
	const char *cs __unused = "foobarnone";

	printf(fmt fmt fmt fmt fmt fmt fmt fmt fmt fmt fmt fmt fmt fmt fmt fmt fmt fmt fmt fmt,
                   "size_t", typename(s),
                "ptrdiff_t", typename(p),
                 "intmax_t", typename(i),
                     "char", typename(c),
              "signed char", typename(sc),
            "unsigned char", typename(uc),
            "char constant", typename('0'), /* xxx the usual conversions give int! */
     "signed char constant", typename((signed char) '0'), /* casts do not!! */
   "unsigned char constant", typename((unsigned char) '0'), /* casts do not!! */
"character string constant", typename("foobar"),
        "const char string", typename(cs),
                        "0", typename(0),
                     "NULL", typename(NULL),
               "0x7FFFFFFF", typename(0x7FFFFFFF), /* sign detection? */
               "0xFFFFFFFF", typename(0xFFFFFFFF), /* sign detection?  */
              "0x7FFFFFFFU", typename(0x7FFFFFFFU),
             "array of int", typename(ai),
           "pointer to int", typename(pi),
                     "true", typename(true), /* XXX the usual conversions give int! */
                     "bool", typename(b)); /* !!! no "usual conversions"! */

	exit(EXIT_SUCCESS);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " CSTD=c11 " fn " && ./" fn))
 * End:
 */
