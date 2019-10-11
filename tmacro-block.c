/*
 * try to understand why "do {} while(0)" might be preferred to any other form
 * of block for a macro with multiple statements that behaves like a function.
 */
#define dwb(p)								\
	do {								\
		int bar = 1;						\
		if (bar > (p)) {					\
			printf("smaller\n");				\
		}							\
		printf("p = %d\n", p);					\
	} while(0)

#define ifb(p)								\
	if (1) {							\
		int bar = 1;						\
		if (bar > (p)) {					\
			printf("smaller\n");				\
		}							\
		printf("p = %d\n", p);					\
	}

#define pb(p)								\
	{								\
		int bar = 1;						\
		if (bar > (p)) {					\
			printf("smaller\n");				\
		}							\
		printf("p = %d\n", p);					\
	}

