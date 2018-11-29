#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 *	Mostly taken from Tony Finch:
 *
 *	Named and optional function arguments in Std-C99
 *	[[by abusing initializers]]
 *	http://fanf.livejournal.com/148506.html
 *
 * [[Perhaps the biggest caveat in use of this trick is that parameters must be
 * referred to as struct members within a function's implementation -- a lesser
 * caveat is that supporting use of positional parameters via positional
 * initializers requires at least one field to have an explictly assigned
 * default value (and this field must be the first defined, and it cannot be
 * given a value as a positional parameter) -- supporting default values can
 * also cause some compilers to complain about overridden initializers -- and of
 * course you also have to declare the parameters struct separately as a
 * function, then use it as a parameter in the function declaration and
 * definition, and also define a macro to hide the detritous of initializing an
 * anonymous struct and make it possible for calls to look more like a normal
 * function calls -- and lastly you can't actually turn off positional parameter
 * simulation, i.e. you can't turn off positional initializers, and yet there
 * are subtle differences between positional initializers and positional
 * arguments in real function calls w.r.t. default type conversions and warnings
 * generated from prototypes, etc.  One big huge advantage though is that new
 * optional parameters can be added to functions without having to update all
 * callers.]]
 */

typedef unsigned int  uint;		/* as it should always have been... */

/*
 * First declare the function's parameters as a structure.
 *
 * Note:  Due to the way the calling macro will provide default values there
 * must be an "optional" or "dummy" parameter as the first field.  This will
 * allow you to call the function with either positional or named arguments, but
 * when using positional arguments no value can be supplied for the first
 * (dummy/optional) parameter.
 */

struct repeat_params {
	void	*appctx;/* must be first and be optional (with a default value) or an un-used dummy */
	char	*str;	/* first usable-as-positional param must be preceded by dummy/default one */
	uint	 n;	/* rest can be used positionally, and can have defaults */
	char	*sep;
};

/*
 * Next, define the function, using the struct as its sole parameter.
 *
 * [[you could make it a pointer, but that just adds more typing and makes
 * references to the struct's fields look even more like struct references]]
 *
 * N.B.:  If the macro below were to be supplied in a common header (used by
 * both the implementation and callers) along with the struct declaration, then
 * it will be necessary to suppress macro expansion by wrapping its name with
 * parenthesis!
 */

static char *
repeat(struct repeat_params p)
{
	char	*r;

	if (p.n < 1 ||
	    p.str == NULL) {
		return NULL;
	}
	r = malloc(((p.n - 1) * strlen(p.sep)) +
	                 (p.n * strlen(p.str)) + 1);
	if (r == NULL) {
		return NULL;
	}
	strcpy(r, p.str);
	while (p.n-- > 1) {	 // accidentally quadratic
		strcat(r, p.sep);
		strcat(r, p.str);
	}

	return r;
}

/*
 * One way to avoid having to use struct references in the function
 * implementation is to define it privately in the traditional way and then call
 * it from a public function using the struct parameter.
 */
static inline char *
repeat2_impl(void	*appctx,
             char	*str,
             uint	 n,
             char	*sep)
{
	char	*r;

	(void) appctx;

	if (n < 1 || str == NULL) {
		return NULL;
	}
	r = malloc(((n - 1) * strlen(sep)) +
	                 (n * strlen(str)) + 1);
	if (r == NULL) {
		return NULL;
	}
	strcpy(r, str);
	while (n-- > 1) {	 // accidentally quadratic
		strcat(r, sep);
		strcat(r, str);
	}

	return r;
}

/* wrapper expands parameters and calls the traditional positional definition */
static char *
repeat2(struct repeat_params p)
{
	return repeat2_impl(p.appctx, p.str, p.n, p.sep);
}


/*
 * Define a wrapper macro that sets the default values using designated
 * initializers and hides away the syntactic rubric allowing the caller to
 * specify just the desired parameters, possibly as new designated
 * initializers, or optionally as positional initializers.
 *
 * This is because designated initializers and positional initializers may be
 * intermixed in the same initializer list.  If a (one or more) positional
 * initializer(s) follow(s) a designated initializer then the positional
 * initializer(s) begin(s) initializing compontnents immediately following the
 * (last) designated initializer.
 *
 * Any field not given a default value, and not overriden with a user-supplied
 * value, will of course be zero-filled.  (e.g. .str will be NULL by default)
 */


/*
 * xxx these next settings should probably be embedded in the macro using the
 * _Pragma operator, and with push/pop wrappers, but trying to do that portably,
 * even with a slew of helper macros, is FAR too complicated to be worthwhile!
 * (e.g. prior to GCC-4.6.0 the push/pop options are not present or are broken)
 */
#if defined(__clang__)
# pragma clang diagnostic ignored "-Winitializer-overrides"
/* warning:  clang (up to and including 8.1.0) pretends to be gcc-4.2.1, thus #elif */
#elif defined(__GNUC__)
/* coincidently 4.2.0 and newer have both the #pragma and the warning */
# if ((__GNUC__ * 100) + (__GNUC_MINOR__ * 10) + __GNUC_PATCHLEVEL__) >= 420
#  pragma GCC diagnostic ignored "-Woverride-init"
# endif
#endif

#define repeat(...)						\
	repeat((struct repeat_params) {				\
	       .n = 1,						\
	       .sep = " ",					\
	       .appctx = NULL,	/* must be last */		\
	       __VA_ARGS__					\
	       })

#define repeat2(...)						\
	repeat2((struct repeat_params) {			\
	        .n = 1,						\
	        .sep = " ",					\
	        .appctx = NULL,	/* must be last */		\
	        __VA_ARGS__					\
	        })

int
main(void)
{
	char *str;

	// Invoke it like these examples (with args in any order)
	str = repeat(.n = 3, .str = "ho");
	puts(str ? str : "(null)");
	free(str);
	str = repeat(.str = "ho", .n = 3);
	puts(str ? str : "(null)");
	free(str);

	// Or equivalently with positional parameters (including optional ones)
	str = repeat("ho", 4, ", ");
	puts(str ? str : "(null)");
	free(str);

	// Note:  as is normal with use of positional parameters, omitting a
	// required argument is likely to give a warning if it has a different
	// type than the second parameter, however use of the initializer macro
	// with positional parameters is even more error prone if not all are
	// actually optional
//	str = repeat(4, ", ");	// N.B.:  sets .str=0x04 (invalid!)
//	puts(str ? str : "(null)");
//	free(str);
	str = repeat(", ");	// N.B.:  probably not what was intended!
	puts(str ? str : "(null)");
	free(str);

	// the "dummy" doesn't have to be a useless dummy
	str = repeat(.appctx = main, .str = "ho", .n = 2);
	puts(str ? str : "(null)");
	free(str);
	// however if specified with positional parameters it must come last and
	// it MUST still be given as a designated initializer (ugly!)
	str = repeat("ho", .appctx = main);
	puts(str ? str : "(null)");
	free(str);

	// if every param is truly optional (i.e. if the default value, be that
	// one supplied in the macro or the default zero initilization) then no
	// args need be supplied
	str = repeat();
	puts(str ? str : "(null)");
	free(str);

	str = repeat2();
	puts(str ? str : "(null)");
	free(str);
	str = repeat2("hum", .appctx = main);
	puts(str ? str : "(null)");
	free(str);
	str = repeat2(.appctx = main, .str = "hum", .n = 2);
	puts(str ? str : "(null)");
	free(str);
	str = repeat2(", ");	// N.B.:  probably not what was intended!
	puts(str ? str : "(null)");
	free(str);
	str = repeat2("hum", 4, ", ");
	puts(str ? str : "(null)");
	free(str);
	str = repeat2(.str = "hum", .n = 3);
	puts(str ? str : "(null)");
	free(str);
	str = repeat2(.n = 3, .str = "hum");
	puts(str ? str : "(null)");
	free(str);

	exit(0);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " CPPFLAGS='-std=c99' " fn " && ./" fn))
 * End:
 */
