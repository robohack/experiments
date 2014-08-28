/*
 * tmktime.c - test mktime(3) -- stolen from emacs-21.2 aclocal.m4
 * 
 * Test program from Paul Eggert (eggert@twinsun.com)
 * and Tony Leneis (tony@plaza.ds.adp.com).
 */
#if defined(BSD) ||							\
	defined(__NetBSD__) ||						\
	defined(__FreeBSD__) ||						\
	defined(__OpenBSD__) ||						\
	defined(__Darwin__) ||						\
	defined(__DragonFly__)
# define HAVE_SYS_TIME_H	/* defined */
# define HAVE_INTTYPES_H	/* defined */
# define HAVE_UNISTD_H		/* defined */
# define HAVE_ALARM		/* defined */
#endif

#ifdef TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# ifdef HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#ifdef __STDC__
/*
 * alternately for the __STDC_VERSION__ test:
 *
 * ((__STDC_VERSION__ - 0) >= 199901L)
 */
# if defined(HAVE_INTTYPES_H) ||					\
	(defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L)
#  include <inttypes.h>
# else
#  error "missing <inttypes.h>"
#  define PRIu64	"llu"		/* assume "long long" */
# endif
#endif

#ifndef HAVE_ALARM	/* i.e. not POSIX */
# define alarm(X)	/* empty */
#endif

static time_t   time_t_max;

static int      verbose = 1;

/* Values we'll use to set the TZ environment variable.  */
static const char *const tz_strings[] = {
	(const char *) 0,			/* XXX does this have the desired effect? */
	"TZ=",					/* not in original test. */
	"TZ=GMT0",
	"TZ=JST-9",
	"TZ=EST+3EDT+2,M10.1.0/00:00:00,M2.3.0/00:00:00"
};

#define N_STRINGS	((int) (sizeof(tz_strings) / sizeof(tz_strings[0])))

/*
 * Fail if mktime fails to convert a date in the "spring-forward" gap.
 * Based on a problem report from Andreas Jaeger.
 *
 * glibc (up to about 1998-10-07) failed this test), NetBSD up to at least 5.1
 * fails this test, as does FreeBSD up to at least 4.7-RC.
 */
static void
spring_forward_gap()
{
	struct tm       tm;
	time_t          t;

	/*
	 * Use the portable POSIX.1 specification "TZ=PST8PDT,M4.1.0,M10.5.0"
	 * instead of "TZ=America/Vancouver" in order to detect the bug even on
	 * systems that don't support the Olson extension, or don't have the
	 * full zoneinfo tables installed.
	 */
	putenv("TZ=PST8PDT,M4.1.0,M10.5.0");

	tm.tm_year = 98;
	tm.tm_mon = 3;
	tm.tm_mday = 5;
	tm.tm_hour = 2;
	tm.tm_min = 0;
	tm.tm_sec = 0;
	tm.tm_isdst = -1;
	errno = 0;
	if (verbose)
		printf("gap time according to asctime() = %s", asctime(&tm));
	if ((t = mktime(&tm)) == (time_t) -1) {
		fprintf(stderr, "mktime(): fails for time in spring-forward gap: %s\n", strerror(errno));
	} else if (verbose)
		printf("gap time as ctime() from mktime() = %s", ctime(&t));
}

/*
 * Fail if mktime fails to convert a date of "Feb 31"
 */
static void
february_non_day()
{
	struct tm       tm;
	time_t          t;

	memset(&tm, 0, sizeof(tm));
	tm.tm_year = 98;
	tm.tm_mon = 1;
	tm.tm_mday = 31;
	tm.tm_hour = 10;
	tm.tm_min = 10;
	tm.tm_sec = 30;
	tm.tm_isdst = -1;
	errno = 0;
	if (verbose)
		printf("Februrary non-day according to asctime() = %s", asctime(&tm));
	if ((t = mktime(&tm)) == (time_t) -1) {
		fprintf(stderr, "mktime(): fails for February non-day: %s\n", strerror(errno));

		return;
	}
	if (verbose)
		printf("Februrary non-day as ctime() from mktime() = %s", ctime(&t));
}

/*
 * leap year test "2000/06/00"
 */
static void
leap_year_non_day()
{
	struct tm       tm;
	time_t          t;

	memset(&tm, 0, sizeof(tm));
	tm.tm_year = 100;
	tm.tm_mon = 5;
	tm.tm_mday = 0;
	tm.tm_hour = 10;
	tm.tm_min = 10;
	tm.tm_sec = 30;
	tm.tm_isdst = -1;
	errno = 0;
	if (verbose)
		printf("leap-year non-day according to asctime() = %s", asctime(&tm));
	if ((t = mktime(&tm)) == (time_t) -1) {
		fprintf(stderr, "mktime(): fails for leap-year non-day: %s\n", strerror(errno));

		return;
	}
	if (verbose)
		printf("leap-year non-day as ctime() from mktime() = %s", ctime(&t));
	if (tm.tm_mon != 4) {
		fprintf(stderr, "mktime(): fails to normalize month for leap-year non-day\n");

		return;
	}
	if (tm.tm_mday != 31) {
		fprintf(stderr, "mktime(): fails to normalize day for leap-year non-day\n");

		return;
	}
}

/*
 * leap year test 2 "2000/03/00"
 */
static void
leap_year_march_zero_day()
{
	struct tm       tm;
	time_t          t;

	memset(&tm, 0, sizeof(tm));
	tm.tm_year = 100;
	tm.tm_mon = 2;
	tm.tm_mday = 0;
	tm.tm_hour = 10;
	tm.tm_min = 10;
	tm.tm_sec = 30;
	tm.tm_isdst = -1;
	errno = 0;
	if (verbose)
		printf("leap-year March zero-day according to asctime() = %s", asctime(&tm));
	if ((t = mktime(&tm)) == (time_t) -1) {
		fprintf(stderr, "mktime(): fails for leap-year March zero-day: %s\n", strerror(errno));

		return;
	}
	if (verbose)
		printf("leap-year March zero -day as ctime() from mktime() = %s", ctime(&t));
	if (tm.tm_mon != 1) {
		fprintf(stderr, "mktime(): fails to normalize month for leap-year March zero-day\n");

		return;
	}
	if (tm.tm_mday != 29) {
		fprintf(stderr, "mktime(): fails to normalize day for leap-year March zero-day\n");

		return;
	}
}

static void
mktime_test(now)
	time_t          now;
{
	struct tm      *lt;

	if ((lt = localtime(&now)) && mktime(lt) != now) {
		fprintf(stderr, "localtime() and mktime() disagree on 'now = %ld'!\n", (long) now);

		return;
	}
	now = time_t_max - now;
	if ((lt = localtime(&now)) && mktime(lt) != now) {
		fprintf(stderr, "localtime() and mktime() disagree on 'time_t_max - now = %ld'!\n", now);

		return;
	}
}

/*
 * Based on code from Ariel Faigon.
 */
static void
irix_6_4_bug()
{
	struct tm       tm;

	tm.tm_year = 96;
	tm.tm_mon = 3;
	tm.tm_mday = 0;			/* XXX [1,31]??? */
	tm.tm_hour = 0;
	tm.tm_min = 0;
	tm.tm_sec = 0;
	tm.tm_isdst = -1;
	mktime(&tm);
	if (tm.tm_mon != 2 || tm.tm_mday != 31) {
		/* XXX also fails on BSDI-1.1 */
		fprintf(stderr, "mktime(): failed irix 6.4 bug\n");

		return;
	}
}

static void
bigtime_test(j)
	int             j;
{
	struct tm       tm;
	time_t          now;

	tm.tm_year = tm.tm_mon = tm.tm_mday = tm.tm_hour = tm.tm_min = tm.tm_sec = j;
	now = mktime(&tm);
	if (now != (time_t) -1) {
		struct tm      *lt = localtime(&now);

		if (!(lt
		      && lt->tm_year == tm.tm_year
		      && lt->tm_mon == tm.tm_mon
		      && lt->tm_mday == tm.tm_mday
		      && lt->tm_hour == tm.tm_hour
		      && lt->tm_min == tm.tm_min
		      && lt->tm_sec == tm.tm_sec
		      && lt->tm_yday == tm.tm_yday
		      && lt->tm_wday == tm.tm_wday
		      && ((lt->tm_isdst < 0 ? -1 : 0 < lt->tm_isdst)
			  == (tm.tm_isdst < 0 ? -1 : 0 < tm.tm_isdst)))) {
			fprintf(stderr, "mktime(): fails bigtime test: j = %d\n", j);

			return;
		}
	} else {
		/*
		 * Some values of j fail -- we don't really care about those --
		 * we only care when it works to know that localtime() gets the
		 * same values back that mktime() used.
		 */
		if (verbose > 1)
			printf("mktime(): failed to convert bigtime: j = %d\n", j);
	}
}

static void
end_of_time_test()
{
	struct tm	tms;
	time_t		t;

	memset(&tms, 0, sizeof(tms));
	tms.tm_year = ~0;
	tms.tm_mday = 1;		/* [1,31], but will be forced */
#if 1
	tms.tm_isdst = -1;
#endif

	errno = 0;
	t = mktime(&tms);
	if (t == -1) {
		if (errno == 0)
			fprintf(stderr, "mktime(): ~0 years failed but did not set errno!\n");
		else if (verbose)
			printf("mktime(): gave expected failure for ~0 years: %s\n", strerror(errno));
	} else {
		if (sizeof(time_t) < sizeof(uint64_t))
			fprintf(stderr, "mktime(): claims to represent ~0 years as %" PRIu64 " seconds since epoch.\n", (uint64_t) t);
		if (errno != 0)
			fprintf(stderr, "mktime(): ~0 years gave errno: %s\n", strerror(errno));
	}
}

/* ARGSUSED */
int
main()
{
	time_t          t,
	                delta;
	int             i,
	                j;

	irix_6_4_bug();
	february_non_day();
	leap_year_non_day();
	leap_year_march_zero_day();
	spring_forward_gap();
	end_of_time_test();

	/*
	 * This test makes some buggy mktime() implementations loop.  Give up
	 * on all tests after 240 seconds.  A mktime() slower than that isn't
	 * worth using anyway.  (XXX BSDI-1.1 on i386 sx25 takes several
	 * minutes -- how long does a VAX 11/750 take to run this?)
	 */
	alarm(240);

	for (time_t_max = 1; 0 < time_t_max; time_t_max *= 2)
		continue;

	time_t_max--;
	delta = time_t_max / 997;		/* a suitable prime number */
	for (i = 0; i < N_STRINGS; i++) {
		if (tz_strings[i])
			putenv(tz_strings[i]);

		for (t = 0; t <= time_t_max - delta; t += delta)
			mktime_test(t);

		mktime_test((time_t) 60 * 60);
		mktime_test((time_t) 60 * 60 * 24);

		for (j = 1; 0 < j; j *= 2)
			bigtime_test(j);

		bigtime_test(j - 1);
	}

	exit(0);
	/* NOTREACHED */
}
