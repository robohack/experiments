
#define ECHO_GUTS(N)							\
	if (echo##N## _in  != 0) {					\
		if (echo_##N == 1)					\
			uart##N##_write(echo##N##_in);			\
		echo##N##_in = 0;					\
	}								\
