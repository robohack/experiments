#include <assert.h>
#include <err.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>

/*
 * parseint() - Using strtol() to convert a string to an "int"
 */ 
int
parseint(const char *str)
{
	char		*ep;
	long int	 lval;
 
	/*
	 * assume the string pointer will never be NULL
	 */
	if (str == NULL) {
		errx(1, "not a number -- null string");
	}
	/*
	 * assume the string will never be empty
	 */
	if (str[0] == '\0') {
		errx(1, "not a number -- empty string");
	}

	/*
	 * must clear errno for this to work (or at least set it to something
	 * other than ERANGE or any other errno value strtol() might return!)
	 */
	errno = 0;

	lval = strtol(str, &ep, 0);	/* octal, decimal, or hexidecimal */

	/*
	 * assume the string will always point to a valid number representation
	 *
	 * XXX strictly this check is not necessary except to differentiate the
	 * error condition....
	 */
	if (ep == str) {
		assert(errno == ERANGE);/* sanity check against strtol() */
		assert(lval == 0);	/* sanity check against strtol() */
		errx(1, "not a number -- no (valid) digits found");
	}

	if (errno) {
		err(1, "strtol()");
	}

	/*
	 * assume the whole string is the number, i.e. caller must pre-trim all
	 * trailing whitespace or any other extraneous characters.
	 */
	if (*ep != '\0') {
		errx(1, "not a number -- trailing garbage found");
	}

#if (INT_MAX < LONG_MAX) || (INT_MIN > LONG_MIN)
	/* also since we only want results valid for "int"... */
	if (lval > INT_MAX) {
    		errno = ERANGE;
		err(1, "maximum is %d", INT_MAX);
	} else if (lval < INT_MIN) {
    		errno = ERANGE;
		err(1, "minimum is %d", INT_MIN);
	}
#endif

	return (int) lval;
}

/*
 * parselong() - Using strtol() to convert a string to a "long"
 */ 
long int
parselong(const char *str)
{
	char		*ep;
	long int	 lval;
 
	/*
	 * assume the string pointer will never be NULL
	 */
	if (str == NULL) {
		errx(1, "not a number -- null string");
	}
	/*
	 * assume the string will never be empty
	 */
	if (str[0] == '\0') {
		errx(1, "not a number -- empty string");
	}

	/*
	 * must clear errno for this to work (or at least set it to something
	 * other than ERANGE or any other errno value strtol() might return!)
	 */
	errno = 0;

	lval = strtol(str, &ep, 0);	/* octal, decimal, or hexidecimal */

	/*
	 * assume the string will always point to a valid number representation
	 *
	 * XXX strictly this check is not necessary except to differentiate the
	 * error condition....
	 */
	if (ep == str) {
		assert(errno == ERANGE);/* sanity check against strtol() */
		assert(lval == 0);	/* sanity check against strtol() */
		errx(1, "not a number -- no (valid) digits found");
	}

	if (errno) {
		err(1, "strtol()");
	}

	/*
	 * assume the whole string is the number, i.e. caller must pre-trim all
	 * trailing whitespace or any other extraneous characters.
	 */
	if (*ep != '\0') {
		errx(1, "not a number -- trailing garbage found");
	}

	return lval;
}

/*
 * parselong_alt() - Using strtoll() to convert a string to a "long"
 */ 
long int
parselong_alt(const char *str)
{
	char		*ep;
	long long int	 qval;
 
	/*
	 * assume the string pointer will never be NULL
	 */
	if (str == NULL) {
		errx(1, "not a number -- null string");
	}

	/*
	 * assume the whole string is the number, i.e. caller must pre-trim all
	 * trailing whitespace or any other extraneous characters.
	 */
	if (str[0] == '\0') {
		errx(1, "not a number -- empty string");
	}

	/*
	 * must clear errno for this to work (or at least set it to something
	 * other than ERANGE or any other errno value strtol() might return!)
	 */
	errno = 0;

	qval = strtoll(str, &ep, 0);	/* octal, decimal, or hexidecimal */

	/*
	 * assume the string will always point to a valid number representation
	 *
	 * XXX strictly this check is not necessary except to differentiate the
	 * error condition....
	 */
	if (ep == str) {
		assert(errno == ERANGE);/* sanity check against strtol() */
		assert(qval == 0);	/* sanity check against strtol() */
		errx(1, "not a number -- no (valid) digits found");
	}

	if (errno) {
		err(1, "strtoll()");
	}

	if (*ep != '\0') {
		errx(1, "not a number -- trailing garbage found");
	}

#if (LONG_MAX < LLONG_MAX) || (LONG_MIN > LLONG_MIN)
	/* also since we only want results valid for "long"... */
	if (qval >= LONG_MAX) {
    		errno = ERANGE;
		err(1, "maximum is %ld", LONG_MAX);
	} else if (qval <= LONG_MIN) {
    		errno = ERANGE;
		err(1, "minimum is %ld", LONG_MIN);
	}
#endif

	return (long int) qval;
}
