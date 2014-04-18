#include <stdio.h>
#include <stdlib.h>

int main(void);

/*
 * Fun and danger with CPP's stringify operator:  '#'
 */
#ifndef ___STRING
/* expand (s), then stringify it */
# define ___STRING(s)	__STRING(s)
#endif
#ifndef __STRING
/* stringify (s) without first expanding it */
# define __STRING(s)	#s
#endif

/*
 * enum names are proper C identifiers, not macros
 */
enum tenum_e {
	ENUM_TO_STRING_1 = 1,
	ENUM_TO_STRING_2,
	ENUM_TO_STRING_3,
	ENUM_TO_STRING_4,
};

int
main()
{
#define EMPTY				/* empty macro body */
	printf("Hello empty %s: %d!\n", __STRING(EMPTY), EMPTY - 0);

#if EMPTY - 0
	printf("Ha HA!  Bet that fooled you!\n");
#endif

	printf("enum named %s has value %d!\n", __STRING(ENUM_TO_STRING_1), ENUM_TO_STRING_1);

#define AN_ENUM	ENUM_TO_STRING_3

	printf("enum named %s has value %d!\n", __STRING(AN_ENUM), AN_ENUM); /* oops, not the enum's name! */
	printf("enum named %s has value %d!\n", ___STRING(AN_ENUM), AN_ENUM); /* expand to show enum's name */

	printf("%s=%s and has value %d!\n", __STRING(AN_ENUM), ___STRING(AN_ENUM), AN_ENUM);
	printf("%s=%s and has value %d!\n",
	       __STRING(AN_ENUM) "." __STRING(AN_ENUM),
	       ___STRING(AN_ENUM) "." ___STRING(AN_ENUM),
	       AN_ENUM);

	/* but it's not needed here because it's already a nested macro...  */
#define PRINT_ENUM_VAL(en)	printf("enum named %s has value %d!\n", __STRING(en), en)
	PRINT_ENUM_VAL(AN_ENUM);

	/* there's no stopping half-way though, i.e. you don't see "DEFINE_TARGET" */
#define DEFINE_TARGET	4
	printf("%s=%s and has value %d!\n", __STRING(DEFINE_TARGET), ___STRING(DEFINE_TARGET), DEFINE_TARGET);

#define A_DEFINE	DEFINE_TARGET
	printf("%s=%s and has value %d!\n", __STRING(A_DEFINE), ___STRING(A_DEFINE), A_DEFINE);

#define STR_DEFINE	__STRING(A_DEFINE)
	printf("%s=%s\n", __STRING(STR_DEFINE), ___STRING(STR_DEFINE));

#define ESTR_DEFINE	___STRING(A_DEFINE)
	printf("%s=%s\n", __STRING(ESTR_DEFINE), ___STRING(ESTR_DEFINE));

	exit(0);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
