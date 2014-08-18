#include <assert.h>
#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

struct tm_dur {
	int	usecs;	/* microseconds [0-999999] */
	int	secs;	/* seconds [0-59] */
	int	mins;	/* minutes [0-59] */
	int	hours;	/* hours [0-23] */
	int	days;	/* days [0-364] */
	int	weeks;	/* weeks [0-51] */
	int	years;	/* years [0-999] */
	int	milleniums;	/* "To infinity and beyond!" */
};

struct tm_us_dfract {
	unsigned int nd;
	unsigned int v;
};

void tm_us_dfract(unsigned int, struct tm_us_dfract *);
void tm_dur_set(struct tm_dur *, double);
char *tm_dur_iso_print(const struct tm_dur *, bool);
char *tm_dur_pretty_print(const struct tm_dur *);


/*
 * given a number of microseconds, return the number of digits of, and the value
 * to print as, an equivalent fractional second, thus trimming all the zeros on
 * the right-hand side and effectively flipping them to the right side, and
 * adding the implied number of zeros necessary to match the magnitude of the
 * original value.
 *
 * 1us -> nd=6, v=1		9us -> nd=6, v=9	55us -> nd=6, v=55
 * 10us -> nd=5, v=1		99us -> nd=6, v=99	550us -> nd=5, v=55
 * 100us -> nd=4, v=1		999us -> nd=6, v=999	5500us -> nd=4, v=55
 * 1000us -> nd=3, v=1		9999us -> nd=6, v=9999	55000us -> nd=3, v=55
 *
 * used to print the fractional second:
 *
 *	struct tm_us_dfract ni;
 *	tm_us_dfract(55000, &ni);
 *	printf("0.%0*u micro-secs", ni.nd, ni.v);	-> "0.000055 micro-secs"
 */
void
tm_us_dfract(unsigned int usec,
	     struct tm_us_dfract *p)
{
	assert(usec < 1e6);
	/*
	 * while the magnitude, beginning at 9, is greater than zero, and the
	 * value is still evenly divisible by 10, decrement the magnitude and
	 * divide the number of nanoseconds by 10.
	 */
	for (p->v = usec, p->nd = 6; p->nd > 0 && p->v % 10 == 0; p->nd--) {
		p->v /= 10;
	}
}

/*
 * hack a struct tm_dur to represent a normalized duration in human terms
 */
void
tm_dur_set(struct tm_dur *dur,
	   double dsecs)
{
	intmax_t secs;
	double isecs;
	double usecs;

	usecs = round(modf(dsecs, &isecs) * 1.0e6);
	memset(dur, 0, sizeof(*dur));
	dur->usecs = (int) usecs;
	secs = (intmax_t) dsecs;
	dur->secs = secs % 60;
	secs /= 60;				/* mins */
	dur->mins = secs % 60;
	secs /= 60;				/* hours */
	dur->hours = secs % 24;
	secs /= 24;				/* days */
	dur->days = secs % 7;
	secs /= 7;				/* weeks */
	dur->weeks = secs % 52;
	secs /= 52;				/* years */
	dur->years = secs % 1000;
	secs /= 1000;				/* milleniums */
	dur->milleniums = secs;

	return;
}

/*
 * Note:  ISO 8601 does not have a concept of "negative" durations.  To
 * properly print a duration which might have negative values one would have to
 * print the absolute value of the duration followed by the end time.
 *
 * No attempt is made to use trancated time representations (5.3.1.4), though
 * reduced precision times (5.3.1.2) are used.
 */
char *
tm_dur_iso_print(const struct tm_dur *dur,
		 bool alt)
{
	char *str;
	bool neg;

	neg = false;
	if (dur->years < 0 || dur->weeks < 0 || dur->days < 0 ||
	    dur->hours < 0 || dur->mins < 0 || dur->secs < 0 || dur->usecs < 0) {
		neg = true;		/* assume one neg means all neg */
	}
	/* xxx this representation of negative durations is non-standard */
	str = NULL;
	asprintf(&str, "%sP", neg ? "-" : "");
	if (dur->years == 0 && dur->weeks == 0 && dur->days == 0 &&
	    dur->hours == 0 && dur->mins == 0 && dur->secs == 0 && dur->usecs < 0) {
		return str;
	}
	if (dur->years != 0 || dur->weeks != 0 || dur->days != 0) {
		if (alt) {
			asprintf(&str, "%s%04d", str, abs(dur->years));
			if (dur->weeks != 0 || dur->days != 0) {
				/* xxx this is probably not strictly standard */
				asprintf(&str, "%s-%02dW", str, abs(dur->weeks));
				if (dur->days != 0) {
					asprintf(&str, "%s-%02d", str, abs(dur->days));
				}
			}
		} else {
			if (dur->years != 0) {
				asprintf(&str, "%s%dY", str, abs(dur->years));
			}
			/* xxx it may not be strictly standard to use weeks */
			if (dur->weeks != 0) {
				asprintf(&str, "%s%dW", str, abs(dur->weeks));
			}
			if (dur->days != 0) {
				asprintf(&str, "%s%dD", str, abs(dur->days));
			}
		}
	}
	if (dur->hours != 0 || dur->mins != 0 || dur->secs != 0 || dur->usecs != 0) {
		asprintf(&str, "%sT", str);
		if (alt) {
			asprintf(&str, "%s%02d:%02d:%02d", str,
				 abs(dur->hours),
				 abs(dur->mins),
				 abs(dur->secs));
		} else {
			if (dur->hours != 0) {
				asprintf(&str, "%s%dH", str, abs(dur->hours));
			}
			if (dur->mins != 0) {
				asprintf(&str, "%s%dM", str, abs(dur->mins));
			}
			if (dur->secs != 0 || dur->usecs != 0) {
				asprintf(&str, "%s%d", str, abs(dur->secs));
			}
		}
		if (dur->usecs != 0) {
			struct tm_us_dfract ni;

			tm_us_dfract((unsigned int) abs(dur->usecs), &ni);
			asprintf(&str, "%s.%0*d", str, ni.nd, ni.v);
		}
		if (!alt && (dur->secs != 0 || dur->usecs != 0)) {
			asprintf(&str, "%sS", str);
		}
	}

	return str;
}

/*
 *	4y-3mo-2d-12:34:56.46894
 *	2y
 *	3mo-2d-12:34:56.46894
 *	2mo
 *	4d-2:04:06.46894	leading zero only on mins & secs
 *	3d			no time
 *	72:34:56.46894		up to 72[:59:59.9] hours
 *	1:04:06.46894		leading zeros still only on mins & secs
 *	1h			no minutes or seconds (or us)
 *	59:56.46894		up to 1 hour
 *	5m			no seconds (or us)
 *	120.46894s		up to 120[.9] seconds
 *	7s			no us
 *	6.46894s		no leading zero
 */
char *
tm_dur_pretty_print(const struct tm_dur *dur)
{
	char *str;
	bool neg;
	bool ps;
	int hrs;
	int secs;

	neg = false;
	if (dur->years < 0 || dur->weeks < 0 || dur->days < 0 ||
	    dur->hours < 0 || dur->mins < 0 || dur->secs < 0 || dur->usecs < 0) {
		neg = true;		/* assume one neg means all neg */
	}
	if (neg) {
		str = strdup("-");	/* i.e. counting down/back? */
	} else {
		str = strdup("");
	}
	ps = false;
	if (dur->years != 0) {
		asprintf(&str, "%s%dy", str, abs(dur->years));
		ps = true;
	}
	if (dur->weeks != 0) {
		asprintf(&str, "%s%s%dwk", str, ps ? "-" : "", abs(dur->weeks));
		ps = true;
	}
	hrs = abs(dur->hours);
	if (abs(dur->days) > 3 || ps) {
		asprintf(&str, "%s%s%dd", str, ps ? "-" : "", abs(dur->days));
		ps = true;
	} else {
		hrs += abs(dur->days) * 24;
	}
	if (hrs != 0) {
		asprintf(&str, "%s%s%d", str, ps ? "-" : "", hrs);
		ps = true;
		if (dur->mins == 0 && dur->secs == 0 && dur->usecs == 0) {
			asprintf(&str, "%sh", str);
			ps = false;
		}
	}
	secs = abs(dur->secs);
	if (abs(dur->mins) > 2 || ps) {
		if (ps) {
			asprintf(&str, "%s:%02d", str, abs(dur->mins));
		} else {
			asprintf(&str, "%s%d", str, abs(dur->mins));
			ps = true;
			if (dur->secs == 0 && dur->usecs == 0) {
				asprintf(&str, "%sm", str);
				ps = false;
			}
		}
	} else {
		secs += abs(dur->mins) * 60;
	}
	if (ps) {
		asprintf(&str, "%s:%02d", str, secs);
	} else if (secs != 0) {
		asprintf(&str, "%s%ds", str, secs);
	}
	if (dur->usecs != 0) {
		struct tm_us_dfract ni;

		tm_us_dfract((unsigned int) abs(dur->usecs), &ni);
		asprintf(&str, "%s.%0*d", str, ni.nd, ni.v);
	}

	return str;
}

int main(int, char *[]);

int
main(int argc,
     char *argv[])
{
	int i;

	for (i = 1; i < argc; i++) {
		double s;
		struct tm_dur dur;
		char *d;

		s = atof(argv[i]);
		printf("secs:    %.06f\n", s);
		tm_dur_set(&dur, s);
		printf("usecs:   %d\t[%d ns]\n", dur.usecs, dur.usecs * 1000);

		d = tm_dur_iso_print(&dur, false);
		printf("iso:     %s\n", d);
		free(d);

		d = tm_dur_iso_print(&dur, true);
		printf("iso-alt: %s\n", d);
		free(d);

		d = tm_dur_pretty_print(&dur);
		printf("pretty:  %s\n", d);
		free(d);

		printf("\n");
	}
	exit(0);
}


/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat (default-value 'compile-command) " LDLIBS=-lm " fn " && ./" fn " 1 5 1999 456.789 9771059213.0000435 29596902.000591 2825869.46"))
 * End:
 */
