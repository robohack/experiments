#include <errno.h>
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHARARR_TOKEN(s)	#s
#define STRINGIFY(en)		CHARARR_TOKEN(en)


int main(void);

int
main()
{
	unsigned int i;
	double gcc_conversion[] = {
		0.500000000000000166533453693773481063544750213623046875,
		3.518437208883201171875e13,
		62.5364939768271845828,
		8.10109172351e-10,
		1.50000000000000011102230246251565404236316680908203125,
		9007199254740991.4999999999999999999999999999999995,
		DBL_MAX,
		DBL_MIN,
		456.789,
		2825869.46,
		INFINITY,
		-1.0 / 0.0,
		0.0 / 0.0,		/* NAN */
	};
	const char *decimal[] = {
		"0.500000000000000166533453693773481063544750213623046875",
		"3.518437208883201171875e13",
		"62.5364939768271845828",
		"8.10109172351e-10",
		"1.50000000000000011102230246251565404236316680908203125",
		"9007199254740991.4999999999999999999999999999999995",
		STRINGIFY(DBL_MAX),
		STRINGIFY(DBL_MIN),
		/* not-verified */
		"456.789",
		"2825869.46",
		"inf",
		"-inf",
		"nan",
	};
	const char *correct[] = {
		"0x1.0000000000002p-1",
		"0x1.0000000000002p+45",
		"0x1.f44abd5aa7ca4p+5",
		"0x1.bd5cbaef0fd0cp-31",
		"0x1.8p+0",
		"0x1.fffffffffffffp+52",
		"(unverified)",
		"(unverified)",
		"(unverified)",
		"(unverified)",
		"inf",
		"-inf",
		"nan",
	};

	for (i = 0; i < sizeof(correct) / sizeof(correct[0]); i++) {
		double df, di, frac;

		printf("%s\n", decimal[i]);
		printf(" Correct =  %s\n", correct[i]);
		printf(" compiler = %a\n", gcc_conversion[i]);
		errno = 0;
		printf(" strtod =   %a\n", strtod(decimal[i], (char **) NULL));
		if (errno != 0) {
			printf("             FAILED: %s\n", strerror(errno));
		}
		errno = 0;
		printf(" strtod =   %f\n", strtod(decimal[i], (char **) NULL));
		if (errno != 0) {
			printf("             FAILED: %s\n", strerror(errno));
		}
		df = modf(strtod(decimal[i], (char **) NULL), &di);
		frac = ceil(df * 1.0e6);
		printf(" modf =     %f, frac.e6:%.0f\n\n", di, frac);
	}

	exit(0);
}


/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " LDLIBS=-lm " fn " && ./" fn))
 * End:
 */
