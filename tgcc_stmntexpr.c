#define MAX(a, b) (			\
	{				\
		__typeof__(a) a_ = (a);	\
		__typeof__(b) b_ = (b);	\
	      a_ > b_ ? a_ : b_;	\
	})
