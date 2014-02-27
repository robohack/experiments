/*
 * a variant of the "classic" GCC implementation of a CPP MAX() macro which
 * avoids multiple expansion of its parameters by first assigning them to
 * private variables inside a GCC "statement expression"
 */
#define max(a, b) (			\
	{				\
		__typeof__(a) a_ = (a);	\
		__typeof__(b) b_ = (b);	\
		(a_ > b_) ? a_ : b_;	\
	})
