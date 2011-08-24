/*
 * see the goodness!
 *
 *	cc -Wall -C -CC -E ttrigraph.c
 */

#define ECHO_GUTS(N)							\
	if (echo##N## _in  != 0) {					\
		if (echo_##N == 1)					\
			uart##N##_write(echo##N##_in);			\
		echo##N##_in = 0;					\
	}								\

#define ECHO_DEFUN(N)							\
	void								\
	echo_##N(void)							\
	{								\
		ECHO_GUTS(N)						\
	}

ECHO_DEFUN(1)

ECHO_DEFUN(2)
