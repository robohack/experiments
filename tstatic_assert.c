#include <assert.h>

#ifndef static_assert			/* XXX must always be a macro in C11 */

/*
 * A macro that can check static expressions which can be used wherever the
 * expression is a valid integer constant expression, but it is not a
 * preprocessor constant expression.
 *
 * Since not all compilers catch all constraint violations, due to either
 * sloppy error checking or language extensions, this variant violates two
 * different kinds of language constraints when the static expression given as
 * a paremeter holds true:
 *
 * 1. it triggers a "negative width in bit-field" error.  Any decent compiler
 *    will spit out the name of the field in its error message.
 *
 * 2. it triggers a "size of array is negative" error.  Any decent compiler
 *    will spit out the name of the array in its error message.
 *
 * One or both of these compile-time errors will then also read like an error
 * message giving the line number where the static_assert() call has been
 * placed.
 *
 * Unfortunately all decent compilers will notice both constraint violations and
 * so most will print two errors for each failing static_assert() invocation;
 * and more modern compilers will further utter secondary messages leading back
 * to the original location of the macro expansion thus making all these tricks
 * to incorporate the line number into the error message unnecessary.
 */

#define static_assert(expr)						\
	typedef struct { \
		int STATIC_ASSERT_FLD_NM_(__LINE__): ((expr) ? 1 : -1);	\
	} STATIC_ASSERT_STRUCT_TYPE_(__LINE__)[(expr) ? 1 : -1];	\
	STATIC_ASSERT_STRUCT_TYPE_(__LINE__) STATIC_ASSERT_VAR_NM_(__LINE__)

#endif /* !defined(static_assert) */

#define STATIC_ASSERT_FLD_NM_(line)	STATIC_ASSERT_FLD_NM_2_(line)
#define STATIC_ASSERT_VAR_NM_(line)	STATIC_ASSERT_VAR_NM_2_(line)
#if (__STDC__ - 0) > 0
# define STATIC_ASSERT_FLD_NM_2_(line)	compile_time_assertion_1t_failed_at_line_##line
# define STATIC_ASSERT_VAR_NM_2_(line)	compile_time_assertion_1v_failed_at_line_##line
#else
# define STATIC_ASSERT_FLD_NM_2_(line)	compile_time_assertion_1t_failed_at_line_/**/line
# define STATIC_ASSERT_VAR_NM_2_(line)	compile_time_assertion_1v_failed_at_line_/**/line
#endif

#define STATIC_ASSERT_STRUCT_TYPE_(line) STATIC_ASSERT_STRUCT_TYPE_2_(line)
#if (__STDC__ - 0) > 0
# define STATIC_ASSERT_STRUCT_TYPE_2_(line) compile_time_assertion_2_failed_at_line_##line
#else
# define STATIC_ASSERT_STRUCT_TYPE_2_(line) compile_time_assertion_2_failed_at_line_/**/line
#endif

/*
 * This is a gross hack to avoid warnings from more modern compilers that will
 * warn about unused typedefs when static_assert() is used within a block of
 * code.
 *
 * Unfortunately it mixes a declaration and a statement in the same macro thus
 * it cannot be used after declarations, nor can it be used in the midst of
 * other declarations.
 */
#define static_assert_in_block(expr)					\
	{typedef struct { \
		int STATIC_ASSERT_FLD_NM_(__LINE__): ((expr) ? 1 : -1);	\
	} STATIC_ASSERT_STRUCT_TYPE_(__LINE__)[(expr) ? 1 : -1];	\
	extern STATIC_ASSERT_STRUCT_TYPE_(__LINE__) STATIC_ASSERT_VAR_NM_(__LINE__); \
	(void) STATIC_ASSERT_VAR_NM_(__LINE__);}


#ifdef the_old_linux_way
/* Force a compilation error if condition is true */
#define BUILD_BUG_ON(condition) ((void)sizeof(char[1 - 2*!!(condition)]))

/* Force a compilation error if condition is true, but also produce a
   result (of value 0 and type size_t), so the expression can be used
   e.g. in a structure initializer (or where-ever else comma expressions
   aren't permitted). */
#define BUILD_BUG_ON_ZERO(e) (sizeof(char[1 - 2 * !!(e)]) - 1)
#endif	/* the_old_linux_way */

#ifdef the_new_linux_way
/* Force a compilation error if condition is true */
#define BUILD_BUG_ON(condition) ((void) BUILD_BUG_ON_ZERO(condition))

/* Force a compilation error if condition is constant and true */
#define MAYBE_BUILD_BUG_ON(cond) ((void) sizeof(char[1 - 2 * !!(cond)]))

/* Force a compilation error if condition is true, but also produce a
   result (of value 0 and type size_t), so the expression can be used
   e.g. in a structure initializer (or where-ever else comma expressions
   aren't permitted). */
#define BUILD_BUG_ON_ZERO(e) (sizeof(struct { int :-!!(e); }))
#define BUILD_BUG_ON_NULL(e) ((void *) sizeof(struct { int :-!!(e); }))
#endif	/* the_new_linux_way */

static_assert(1 == 1);

#if 0
static_assert(1 == 0);
#endif

/* make sure compiler isn't on drugs! */
static_assert(sizeof(char) == 1);
static_assert(sizeof(unsigned char) == 1);
static_assert(sizeof(signed char) == 1);
static_assert(sizeof(char) < sizeof(int)); /* make sure EOF is valid! */

#include <stdio.h>
#include <stdlib.h>

int main(void);

int
main()
{
	int foo = 0;
#if 1
	int c;
#endif
#if 0
	/* xxx without C11 static_assert() this will give a warning about an unused variable */
	static_assert(sizeof(long) > 1);
	static_assert(sizeof(char *) > 1);
#else
	static_assert_in_block(sizeof(long) > 1);
	static_assert_in_block(sizeof(char *) > 1);
#endif
#if 0
	int c;
#endif


	if (foo) {
		foo = 0;		/* xxx GCC: "will never be executed" */
	}

	/*
	 * in theory pre-C99 compilers should complaint or error-out here
	 * because this introduces a declaration in the midst of code,
	 * i.e. after the first non-declaration statement in a block.
	 *
	 * ISO C90, C89, and K&R all forbid mixing declarations and code.
	 */
#if 0
	static_assert(1 == 0);
#endif

	while ((c = fgetc(stdin)) != EOF) {
		fputc(c, stdout);
	}
	if (ferror(stdin)) {
		perror("stdin");
		exit(1);
	}
	/*
	 * NOTE: the most portable alternative for the loop above is as below.
	 * This is needed for systems where sizeof(int)==1.  However note that
	 * a hosted implementation of C, i.e. one implementing a compliant
	 * version of the <stdio.h> interfaces, could not strictly exist on any
	 * system where sizeof(int)==1, since then either binary streams would
	 * be impossible (one value of an int (-0 on one's complement?) would
	 * have to be reserved for EOF), or EOF would not actually indicate EOF
	 * or error as is required by the standard.
	 *
	 * C11 (Draft N1570) 7.21.7.1 p3 clearly says that EOF indicates an error:
	 *
	 *	If the end-of-file indicator for the stream is set, or if the
	 *	stream is at end-of-file, the end-of-file indicator for the
	 *	stream is set and the fgetc function returns EOF.  Otherwise,
	 *	the fgetc function returns the next character from the input
	 *	stream pointed to by stream.  If a read error occurs, the error
	 *	indicator for the stream is set and the fgetc function returns
	 *	EOF.
	 *
	 * (IFF sizeof(int) == 1 then chances are one time out of 2^CHAR_BIT
	 * times that feof() and ferror() will have to be called, i.e. only
	 * when fgetc() returns EOF)
	 */
	while (((c = fgetc(stdin)) != EOF) || (!feof(stdin) && !ferror(stdin))) {
		fputc(c, stdout);
	}
	exit(foo);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " CPPFLAGS=-pedantic " fn " && ./" fn " < /dev/null && echo foo | ./" fn))
 * End:
 */
