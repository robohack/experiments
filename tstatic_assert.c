#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#if !defined(__GNUC_PREREQ__)
# if defined(__GNUC__)
#  define __GNUC_PREREQ__(x, y)						\
	((__GNUC__ == (x) && __GNUC_MINOR__ >= (y)) ||			\
	 (__GNUC__ > (x)))
# else
#  define __GNUC_PREREQ__(x, y)	0
# endif
#endif

/*
 * __unused: Note that item or function might be unused.
 */
#if !defined(__unused)
# if __GNUC_PREREQ__(2, 7)
#  define __unused	__attribute__((__unused__))
# else
#  define __unused	/* delete */
# endif
#endif


#ifndef static_assert			/* XXX must always be a macro in C11 */

/*
 * A macro that can check static expressions which can then be used wherever the
 * expression is a valid integer constant expression, but it is not a
 * preprocessor constant expression (i.e. cannot be used in preprocessor tests).
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
 * Unfortunately all good and decent compilers will notice both constraint
 * violations and so most will print two errors for each failing static_assert()
 * invocation; and more modern compilers will further utter secondary messages
 * leading back to the original location of the macro expansion thus making all
 * these tricks to incorporate the line number into the error message
 * unnecessary.
 */

# define static_assert(expr, msg)						\
	typedef struct { 							\
	int STATIC_ASSERT_FLD_NM_(__LINE__): (/*CONSTCOND*/(expr) ? 1 : -1);	\
	} STATIC_ASSERT_STRUCT_TYPE_(__LINE__)[/*CONSTCOND*/(expr) ? 1 : -1]	\
	__unused

#endif /* !defined(static_assert) */

#define STATIC_ASSERT_FLD_NM_(line)	STATIC_ASSERT_FLD_NM_2_(line)
#if (__STDC__ - 0) > 0
# define STATIC_ASSERT_FLD_NM_2_(line)	compile_time_assertion_b_failed_at_line_##line
#else
# define STATIC_ASSERT_FLD_NM_2_(line)	compile_time_assertion_b_failed_at_line_/**/line
#endif

#define STATIC_ASSERT_STRUCT_TYPE_(line) STATIC_ASSERT_STRUCT_TYPE_2_(line)
#if (__STDC__ - 0) > 0
# define STATIC_ASSERT_STRUCT_TYPE_2_(line) compile_time_assertion_a_failed_at_line_##line
#else
# define STATIC_ASSERT_STRUCT_TYPE_2_(line) compile_time_assertion_a_failed_at_line_/**/line
#endif


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

static_assert(1 == 1, "good, CPP agrees at least one integer equates with itself");

#if 0
static_assert(1 == 0, "demonstrating static assertion failure");
#endif

/* make sure compiler isn't on drugs! */
static_assert(sizeof(char) == 1, "char must be one byte!");
static_assert(sizeof(unsigned char) == 1, "unsigned char must be one byte!");
static_assert(sizeof(signed char) == 1, "signed char must be one byte!");
static_assert(sizeof(char) < sizeof(int), "int is not big enough to represent EOF!"); /* make sure EOF is valid! */

int main(void);

int
main()
{
	int foo = 0;
	static_assert(sizeof(long) > 1, "long is too small");
	static_assert(sizeof(char *) > 1, "char pointer is too small");
	int c;


	if (foo) {
		foo = 0;		/* xxx GCC: "will never be executed" */
	}

	/*
	 * in theory pre-C99 compilers, and pedantic ISO C90 or newer compilers,
	 * will complain or error-out here when using the pre-C11
	 * static_assert() variant because this introduces a declaration in the
	 * midst of code, i.e. after the first non-declaration statement in a
	 * block.
	 *
	 * ISO C90, C89, and K&R all forbid mixing declarations and code.
	 */
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ - 0) >= 201112L	/* xxx there are compiler specific tests for _Static_assert support */
	static_assert(1 == 1, "demonstrating static assertion within code");
#endif

	while ((c = fgetc(stdin)) != EOF) {
		fputc(c, stdout);
	}
	if (ferror(stdin)) {
		perror("stdin error");
		exit(1);
	} else if (!feof(stdin)) {
		perror("stdin !EOF");
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
