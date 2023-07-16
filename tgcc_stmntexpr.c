#if !defined(__GNUC__) || (defined(__GNUC__) && defined(__STRICT_ANSI__))
# define typeof	__typeof__
#endif
/*
 * a variant of the "classic" GCC implementation of a CPP MAX() macro which
 * avoids multiple expansion of its parameters by first assigning them to
 * private variables inside a GCC "statement expression"
 */
#define max(a, b) (			\
	{				\
		typeof(a) a_ = (a);	\
		typeof(b) b_ = (b);	\
		(a_ > b_) ? a_ : b_;	\
	})
