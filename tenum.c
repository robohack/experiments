/*
 * WARNINGS!!!! WARNINGS!!!! WARNINGS!!!!
 *
 * The size of an enum type may vary between compilers and hosts (even though
 * they _should_ always be ints)!  I.e. DO NOT use them to type function
 * parameters!
 *
 * enum constants are signed integers (int)!
 *
 * You can't print enumeration values symbolically without writing explicit
 * code to translate them into text.
 *
 * There is no prohibition on assigning enum typed variables some random
 * integer value not represented by a related enum constant.  (Thus it may not
 * always be wise to use enum typed variables!)
 *
 * It may be possible to mix enum constants arbitrarily between enum typed
 * variables without warning.
 */

/*
 * Whenever you need to define a set of mutually exclusive states, or other
 * coded values (e.g. unique error codes), always use an enum (with automatic
 * initialization, though perhaps starting at some offset from zero).
 *
 * Here the compiler ensures each name has a monotonically increasing, and thus
 * unique, value.
 *
 * Further a good compiler will supply these names to the debugger so that
 * easily interpreted symbolic names can be supplied to the user.
 *
 * It's a very good idea to make sure enum names are all upper case with
 * underscores ('_') for word separators.
 */
typedef enum state_e {
	FIRST = 1,
	SECOND,
	THIRD,
	FOURTH
} states_t;

/*
 * NOTE:  keep in mind that for an enum to be useful it has to have a wide
 * enough scope to cover all its uses, so beware of your naming conventions!
 */

/*
 * enum can also be used for defining bit flag values, but they have almost as
 * many drawbacks as using plain macros, especially as compared to bitfields.
 *
 * (see tbitfields-as-flags.c for an almost infinitely better way!!!)
 *
 * Note also that a debugger may not decode these values into symbolic names.
 *
 * WARNING:  take great care to cast enum type variables when using them in
 * integer expressions, and remember that they are sometimes[*] signed integer
 * values!  (so it's probably best to cast them to (unsigned int))
 *
 * Some compilers do not premit conversion between enumerated types and integers
 * without proper use of a cast.
 *
 * [*] GCC:  "Normally, the type is unsigned int if there are no negative values
 * in the enumeration, otherwise int"
 *
 * WARNING:  take even greater care to make sure you give each a unique value,
 * using shifts as shown.  Watch out for missing or duplicate increments in the
 * shift values!
 *
 * IMPORANT WARNING:  Do not use enums for bit flags unless you are stuck with
 * a legacy API that demands such anacronisms!!!  This is simply the better way
 * than using preprocessor macros.  NEVER use preprocessor macros unless they
 * are hard-coded into an existing API.
 */
typedef enum flags_e {
	BIT_ONE = (1U << 0),
	BIT_TWO = (1U << 1),
	BIT_THREE = (1U << 2),
	BIT_FOUR = (1U << 3)
} flags_t;

typedef enum more_flags_e {
	BIT_FIVE = (1U << 4),
	BIT_SIX = (1U << 5),
	BIT_SEVEN = (1U << 6),
	BIT_EIGHT = (1U << 7)
} more_flags_t;

#include <unistd.h>

/*
 * NOTICE:  IFF the API does stupidly/historically require preprocessor macros
 * then you can, and should, implement them like this (only the compiler and OS
 * should really use the leading underscores since that's the mechanism reserved
 * by the language definition to avoid conflict with user identifiers, and so
 * abusing it for user identifiers risks conflicting with some other
 * implementation's internals).
 *
 * XXX Of course this doesn't usually help much with debugging unless the
 * objects and/or parameters declared to use these values are also declared with
 * the appropriate enum (or its typedef helper), as debuggers will not by
 * default look up enum identifiers for arbitrary integer values (as that would
 * be stupid).  Of course if you tell the debugger to co-erce the type of the
 * int variable into the appropriate enum type, then it should do the lookup.
 */
typedef enum __WHENCE_T {
	__my_SEEK_CUR,
	__my_SEEK_END,
	__my_SEEK_SET
} __whence_t;

#define MY_SEEK_CUR __my_SEEK_CUR
#define MY_SEEK_END __my_SEEK_END
#define MY_SEEK_SET __my_SEEK_SET

#include <inttypes.h>

typedef enum error_e {
	ERROR_NOERROR = 0,
	ERROR_BYTE_OVERFLOW = -(1 << 0),
	ERROR_INPUT_OVERFLOW = -(1 << 1),
	ERROR_INVALID_SYMBOL = -(1 << 2),
	ERROR_NOT_ENOUGH_INPUT = -(1 << 3),
} error_t;

typedef enum {
	kRefrigerator = 600,
	kToaster = 700,
	kBlender = 800
} appliance_type_t;

typedef enum {
	k120vac = 0,
	k240vac = 1,
	k12vdc = 2
} voltage_t;

typedef struct {
	appliance_type_t type:16;
	voltage_t voltage:8;
	uint16_t crc;
} __attribute__((__packed__)) appliance_t;

/*
 * High authorities(*) on good C style and overall programming practices
 * suggest using members of an un-named (or anonymous) enum for all numerical
 * constants as well.
 *
 * (i.e. instead of using #define preprocessor macros which change the lexical
 * structure of the program underfoot).
 *
 * Also, though C (now) has "const" which can be applied to initialized int
 * variables, such variables cannot be used as array bounds thus giving another
 * benefit to always using enums for numerical constants.
 *
 * (*) K&P in The Practice of Programming
 *
 * Note that a debugger won't always decode these values sensibly.
 */
enum {
	CONSTANT = 1,
	ANOTHER = 42
};

/*
 * note GCC allows:   enum __attribute__ ((__packed__)) my_enum {...};
 *
 * This should cause the compiler to choose the smalles possible integer
 * representation for the enum values (sort of like an enum-specific version of
 * the -fshort-enum option).
 */

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

states_t func(flags_t);			/* xxx don't do this! */
int main(void);

states_t
func(flags_t flag)			/* xxx don't do this! */
{
	more_flags_t mf = flag;		/* clang-425.0.27 warns */

	if (mf == BIT_FOUR)
		return BIT_EIGHT;	/* clang-425.0.27 & 1.7 warns */

	return ANOTHER;			/* clang-1.7 warns */
}

static void
appliances(int foo)
{
	appliance_t someapp;

	someapp.type = foo;		/* conversion to 'short unsigned int' from int */
	someapp.voltage = 4;

	printf("sizeof(appliance_t) = %lu\n", sizeof(someapp));
//	printf("sizeof(appliance_t.type) = %lu\n", sizeof(someapp.type));
//	printf("sizeof(appliance_t.voltage) = %lu\n", sizeof(someapp.voltage));
	printf("sizeof(appliance_t.crc) = %lu\n", sizeof(someapp.crc));

//	printf("offsetof(appliance_t.type) = %lu\n", offsetof(appliance_t, type));
//	printf("offsetof(appliance_t.voltage) = %lu\n", offsetof(applicance_t, voltage));
	printf("offsetof(appliance_t.crc) = %lu\n", offsetof(appliance_t, crc));

	return;
}

int
main()
{
	states_t sv = SECOND;
	unsigned int fv = (unsigned int) (BIT_ONE | BIT_TWO | BIT_THREE | BIT_FOUR);
	flags_t efv = BIT_ONE | BIT_TWO | BIT_THREE | BIT_FOUR;
	more_flags_t emfv = BIT_ONE | BIT_SIX | BIT_THREE | BIT_EIGHT;
	int cv = ANOTHER;

	printf("error = %d\n", ERROR_NOT_ENOUGH_INPUT);

	printf("sizeof(sv) = %zd, sizeof(efv) = %zd\n", sizeof(sv), sizeof(efv));
	printf("sv = %u, fv = 0x%0*x, efv = 0x%0*x, emfv = 0x%0*x, cf = %d\n",
	       (unsigned int) sv,
	       (int) sizeof(unsigned int), (unsigned int) fv,
	       (int) sizeof(unsigned int), (unsigned int) efv,
	       (int) sizeof(unsigned int), (unsigned int) emfv,
	       cv);

#ifdef __GNUC__
	printf("flags_t compat more_flags_t? -- %s\n", __builtin_types_compatible_p(flags_t, more_flags_t) ? "yes" : "no");
	printf("states_t compat flags_t? -- %s\n", __builtin_types_compatible_p(flags_t, more_flags_t) ? "yes" : "no");
#endif

	sv = func(BIT_THREE | BIT_ONE);
	/*
	 * compilers which can warn about enum comparison and conversion mixups
	 * won't usually warn about enum vs. int comparisons or conversions,
	 * since "the usual conversions" will always turn an enum into an int
	 * in any such expression.
	 */
	sv = func(CONSTANT);		/* no warning, has matching value */
	cv = func(BIT_FOUR);		/* result converts to int */
	fv = func(BIT_ONE);		/* result converts to uint */

	sv = func(ANOTHER);		/* clang-1.7 warns */
	sv = func(BIT_EIGHT);		/* clang-425.0.27 & 1.7 warns */
	sv = func(BIT_THREE | BIT_EIGHT); /* clang-1.7 warns */
	efv = func(BIT_ONE);		/* clang-425.0.27 warns */
	emfv = func(BIT_ONE);		/* clang-425.0.27 warns */

	if (func(BIT_FOUR) == CONSTANT)
		printf("really?\n");
	if (func(BIT_FOUR) == ANOTHER)
		printf("fur sure?\n");

	switch (func(BIT_ONE)) {	/* all warn about missing states */
	case BIT_ONE:			/* no warning! */
	case BIT_TWO:			/* no warning! */
	case BIT_EIGHT:			/* all warn not in states_t */
		printf("in the first 3\n");
		break;
	}

	switch (func(BIT_ONE)) {	/* all warn about missing state */
	case FIRST:
	case SECOND:
	case THIRD:
		printf("in the first 3\n");
		break;
	}

	switch (func(BIT_ONE)) {
	case FIRST:
	case SECOND:
	case THIRD:
		printf("one of the first 3\n");
		break;
	default:
		printf("default\n");
		break;
	}

	appliances(4);

	exit(0);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
