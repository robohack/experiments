#include <stdbool.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef union memalign_u {
	char a;
	short b;
	int c;
	long d;
	long long e;
	float f;
	double g;
	long double h;
	void *i;
	void (*j)(void);
	union memalign_u *k;
} memalign_t;

#define alignof(type)   (sizeof(struct { char d; type x; }) - sizeof(type))

/*
 * thus
 *
 * 	alignof(memalign_t)
 *
 * is the size in bytes of the maximally aligned type
 *
 * but sadly you can't do that in CPP alone.
 */

/*
 * a structure unioned with array of intmax_t should be aligned well enough to
 * allow access to any possible member type (except maybe float types).
 */

/*
 * to increment a pointer to the next alignment:
 */

#if 0
	ptr = (ptr + __BIGGEST_ALIGNMENT__) & ~(__BIGGEST_ALIGNMENT__);
#endif

/*
 * struct str types
 */
enum str_type_e {
	STR_UNKNOWN = 0,
	STR_LITERAL = 0xF01010F0,		/* use only ALS() to access */
	STR_STATIC = 0xBDE0E0BD,
	STR_DYNAMIC = 0xF40505F4,
	STR_CONTAINER = 0xF5ACACF5,
};

/*
 *  XXX This could probably use some further consideration.....
 *
 *  It must not be a valid UTF-8 string, nor can it contain an initial NUL byte.
 *
 *  Maybe it would best be just high-bit control characters?
 *
 *  It does not really have to be exactly 4 bytes.
 *
 */
#define STR_LITERAL_PREFIX	"\360\020\020\360" /* 0xF01010F0 */

/* data type for a dynamic string region */
struct str {
	enum str_type_e t;
	const char *p;			/* text region (declared const to avoid warnings) */
	/* the rest are valid and used only if type == STR_DYNAMIC */
	size_t i;			/* index at end of used text region */
	size_t a;			/* current allocation size */
};

#ifndef __UNCONST
# define __UNCONST(a)	((void *)(uintptr_t)(const void *)(a))
#endif

#ifdef USE_STR_LITERAL
/* literal str setter */
# ifdef __clang__
/* Clang rightfully warns about the lack of alignment, but we don't care */
#  define S(s)								\
	_Pragma("clang diagnostic push")				\
	_Pragma("clang diagnostic ignored \"-Wcast-align\"")		\
	((const struct str *) STR_LITERAL_PREFIX s)			\
	_Pragma("clang diagnostic pop")
# else
/* GCC complains about the Clang pragma statements */
#  define S(s)								\
	((const struct str *) STR_LITERAL_PREFIX s)
# endif
#endif
/* literal str getter */
#define ALS(s)	((const char *) s + sizeof(((struct str *) NULL)->t))

/*
 * "auto" (stack) struct str's can be defined with SC(), from any
 * NUL-terminated array of chars (literal, auto, static, heap).
 *
 * They are of course only valid in the current stack frame.
 *
 * They can be used whenever calling a function that takes a struct str *, but
 * you only have a [const] char * and you don't want to mess with allocating a
 * struct str.
 */
#define SC(s)	(& ((const struct str) {.t = STR_CONTAINER, .p = (s)}))

/*
 * secret private static strings!
 *
 * This works because you can put a compound statement into an expression in
 * GCC and the value of the last statement in the block is returned as the
 * expression's value.  (so-called "statement expressions", available in most
 * major compilers)
 *
 * The only limitation is this cannot be used as an initializer at file-level.
 */
#define SS(s)	(								\
	{									\
		static const struct str pstr = {.t = STR_STATIC, .p = (s)};	\
		&pstr;								\
	})

enum str_type_e
str_type(const struct str *);

enum str_type_e
str_type(const struct str *unk_str)
{
#ifdef USE_STR_LITERAL
	const unsigned char *p0;
	const unsigned char *p;
#endif
	unsigned int type;

#ifdef DEBUG
	assert(sizeof(type) == sizeof(unk_str->t));
#endif

#ifdef USE_STR_LITERAL
	/*
	 * Since it is not possible to specify the alignment of string
	 * literals, accessing s blindly as a struct when it has been produced
	 * by the S() macro above, and in particular the integer s->t, may
	 * result in an un-aligned access, and thus SIGBUS, on some platforms.
	 *
	 * In fact recent versions of Clang's "-fcatch-undefined-behavior"
	 * option will generate an illegal instruction for this dereference just
	 * to prove the point.
	 *
	 * Apple LLVM version 5.0 (clang-500.2.79) (based on LLVM 3.3svn)
	 * Target: x86_64-apple-darwin13.0.0
	 *
	 * The newer "-fsanitize=undefined" option will by default print a
	 * warning at runtime.
	 *
	 * So, instead of acessing it directly we read it out byte-by-byte.
	 *
	 * Note we do not have to do this dance if we only use STR_STATIC and
	 * never STR_LITERAL.
	 */
	p0 = (const unsigned char *) unk_str;
	type = 0;
	for (p = p0; (size_t) (p - p0) < sizeof(unk_str->t); p++) {
#ifdef DEBUG
		fprintf(stderr, "str_type(): @%u, *p = 0x%02x, type = 0x%08x\n",
			(unsigned int) (p - p0),
			(unsigned int) *p,
			type);
#endif
		if (*p == '\0') {
			break;
		}
		type <<= 8;
		type |= (unsigned int) *p;
	}
#else
	type = unk_str->t;
#endif

#ifdef DEBUG
	fprintf(stderr, "str_type(): type = 0x%08x\n", type);
#endif

	switch (type) {
	case STR_DYNAMIC:
	case STR_LITERAL:
	case STR_CONTAINER:
	case STR_STATIC:
	case STR_UNKNOWN:
		break;
	default:
#ifdef DEBUG
		assert(1 == 0);
#endif
		return STR_UNKNOWN;
	}

	return type;
}

/* a getter to optionally wrap around ALS() */
#ifdef USE_STR_LITERAL
# define AS(str) ((str_type(str) == STR_LITERAL) ? ALS(str) : str->p)
# else
# define AS(str) ((str)->p)
#endif

/* STR_BUMP - the basic quantum of allocation space */
#define STR_BUMP	(64)

struct str *
str_init(const char *);

struct str *
str_init(const char *cs)
{
	struct str *d_str;

	d_str = malloc((size_t) STR_BUMP);
	if (d_str == NULL) {
		return NULL;
	}

	d_str->t = STR_DYNAMIC;
	d_str->a = ((strlen(cs) + 1) / STR_BUMP) + STR_BUMP;
	d_str->p = malloc(d_str->a);
	if (d_str->p == NULL) {
		free(d_str);

		return NULL;
	}
	strcpy(__UNCONST(d_str->p), cs);
	d_str->i = strlen(d_str->p);

	return d_str;
}

void
str_free(struct str *);

void
str_free(struct str *s)
{
	if (s) {
		if (s->p) {
			free(__UNCONST(s->p));
		}
		free(s);
	}

	return;
}

void
print_str_info(const struct str *,
	       long);

void
print_str_info(const struct str *s,
	       long max)
{
	const struct str *un_str = SC("un-");

	printf("\n%saligned struct str at %p, (off by %ju)\n",
	       (((uintptr_t) s % (uintptr_t) max) == 0) ? "" : AS(un_str),
	       s,
	       (uintmax_t) ((uintptr_t) s % (uintptr_t) max));

	switch (str_type(s)) {
	case STR_DYNAMIC:
		printf("string contains DYNAMIC content: '%s', i=%ju, s=%ju\n", s->p, (uintmax_t) s->i, (uintmax_t) s->a);
		break;
	case STR_STATIC:
		printf("string contains STATIC content: '%s'\n", s->p);
		break;
	case STR_CONTAINER:
		printf("string contains CONTAINER content: '%s'\n", s->p);
		break;
	case STR_LITERAL:
		printf("string contains LITERAL content: '%s'\n", ALS(s));
		break;
	case STR_UNKNOWN:
		printf("string contains UNKNOWN content at %p\n", s);
		break;
	}

	return;
}

void
warn_char_to_struct_align(long);

void
warn_char_to_struct_align(long max)
{
	const struct str *a_str = SC("this is an auto string");
#ifdef USE_STR_LITERAL
	const struct str *l_str = S("this is a literal string");
#endif
	const struct str *s_str = SS("this is a static string");
	struct str *d_str;

	d_str = str_init("dynamic str (inited from a literal)");

	print_str_info(a_str, max);
#ifdef USE_STR_LITERAL
	print_str_info(l_str, max);	/* un-aligned l_str */
#endif
	print_str_info(s_str, max);
	print_str_info(d_str, max);

	str_free(d_str);

	return;
}

void
demo_unaligned_access(long);

void
demo_unaligned_access(long max)
{
#define UA_STR	(STR_LITERAL_PREFIX "a hopefully un-aligned struct")
#ifdef __clang__
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wcast-align"
#endif
	const struct str *r_str = (const struct str *) UA_STR;
#ifdef __clang__
# pragma clang diagnostic pop
#endif
	char *trick;
	struct str *ua_str;

	trick = malloc(sizeof(UA_STR) + ((unsigned long) max - 1));
#if defined(__clang__)
	/* XXX: expect clang -Wcast-align to warn of required alignment change */
# if (__clang_major__ >= 5)		/* 5, or 4.something */
	printf("\nIf you've compiled with -fcatch-undefined-behaviour, expect a crash now....\n");
# endif
#endif
	/*
	 * Clang warns:
talign.c:345:11: warning: cast from 'char *' to 'struct str *' increases required alignment from 1 to 8 [-Wcast-align]
	*/
	ua_str = (struct str *) (trick + (max - 1));
	memcpy(ua_str, r_str, sizeof(UA_STR));

	/*
	 * For the switch() expr below Apple LLVM version 8.1.0 (clang-802.0.42)
	 * with -fsanitize=undefined says:
	 *
talign.c:371:18: runtime error: member access within misaligned address 0x7fbce840241f for type 'struct str', which requires 8 byte alignment
0x7fbce840241f: note: pointer points here
 e8 bc 7f 00 f0  10 10 f0 61 20 68 6f 70  65 66 75 6c 6c 79 20 75  6e 2d 61 6c 69 67 6e 65  64 20 73
             ^ 
talign.c:371:18: runtime error: load of misaligned address 0x7fbce840241f for type 'enum str_type_e', which requires 8 byte alignment
0x7fbce840241f: note: pointer points here
 e8 bc 7f 00 f0  10 10 f0 61 20 68 6f 70  65 66 75 6c 6c 79 20 75  6e 2d 61 6c 69 67 6e 65  64 20 73
             ^ 
	 */
	printf("\n%saligned struct str @%p, (off by %ld)\n",
	       (((uintptr_t) ua_str % (uintptr_t) max) == 0) ? "" : "un-",
	       ua_str, (long) ((uintptr_t) ua_str % (uintptr_t) max));

#ifndef __clang__
	printf("If running on alignment sensitive platform, expect a SIGBUS now....\n");
#endif
	switch (ua_str->t) {
	case STR_DYNAMIC:
		printf("DYNAMIC ua_str: %s\n", ua_str->p);
		break;
	case STR_CONTAINER:
		printf("CONTAINER ua_str: %s\n", ua_str->p);
		break;
	case STR_STATIC:
		printf("STATIC ua_str: %s\n", ua_str->p);
		break;
	case STR_LITERAL:
		printf("LITERAL ua_str: %s\n", ALS(ua_str));
		break;
	case STR_UNKNOWN:
		printf("UNKNOWN ua_str\n");
		break;
	}

	free(trick);

	return;
}

/*
 * Originally from:
 *
 * maxalign.c
 *	A program that prints -DMAXALIGN=n, when the storage alignment
 *	is not equal to sizeof (union align); see src/arena.c and
 *	src/memchk.c.
 *	Dave Hanson / drh@microsoft.com
 *	created  Fri Jul 10 16:35:53 EDT 1998 by drh
 */

/*
 * On most platforms, malloc returns pointers to blocks that are aligned on
 * addresses that are multiples of the size of the largest basic data type.
 * Some code uses a union to determine this multiple.
 *
 * However alignments are less restrictive on some platforms and for these
 * ALIGN can be defined as the alignment required.
 *
 * This program attempts to determine the correct value for ALIGN, if one is
 * necessary, and echo the appropriate CPP line.
 *
 * Unfortunately, the method used relies on the C compiler using the same
 * alignments as malloc(), which is not required.
 *
 * malloc() is the final authority:  If it returns addresses that are multiples
 * of sizeof(union align), then ALIGN is unnecessary; otherwise, ALIGN must
 * provide the alignment.
 *
 * Incorrect values of ALIGN can cause crashes and assertion failures.
 */
union align {
	char c;
	int i;
	long l;
	long *lp;
	void *p;
	void (*fp)(void);
	float f;
	double d;
	long double ld;
};

typedef void (*functp);
#define yy \
	xx(char, c);\
	xx(int, i);\
	xx(long, l);\
	xx(long *, lp);\
	xx(void *, p);\
	xx(functp, fp);\
	xx(float, f);\
	xx(double, d);\
	xx(long double, ld);

int
main(void);

int
main()
{
	long max = 0;
#define xx(t,v)	struct { char pad; t v; } v
	yy;
#undef xx

#define xx(t,v) if ((char *)&v.v - &v.pad > max)			\
		max = (char *)&v.v - &v.pad
	yy;
#undef yy

	assert(max > 0);

	printf("\nsizeof(union align) = %ju\t(naive alignment)\n", (uintmax_t) sizeof(union align));

	if (max != sizeof(union align)) {
		printf("#define ALIGN\t%ld\t/* calculated ideal alignment */\n", max);
	} else {
		printf("#define ALIGN\tsizeof(union align)\n");
	}
#ifdef __BIGGEST_ALIGNMENT__
	printf("Compiler says:\t__BIGGEST_ALIGNMENT__ = %d", __BIGGEST_ALIGNMENT__);
#endif

	/*
	 * XXX hmmm..... Clang 5.0 says:
	 *
	 * warning: cast from 'char *' to 'struct str *' increases required alignment from 1 to 8
	 *
	 * Suggesting that the minimum alignment is 8, but "max" = 16
	 */

	warn_char_to_struct_align(max);

	demo_unaligned_access(max);

	exit(EXIT_SUCCESS);
	/* NOTREACHED */
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
