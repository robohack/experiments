#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * strreverse() -- reverse a string "in place".
 * 
 * Assume 'start' points to a valid, writable, NUL-terminated string.
 */
static void
strreverse(char *start)
{
	char *end = start;

	while (*end != '\0') {
		end++;
	}
	/* note 'end' now points past the end of the string, at the NUL */
	while (start < end) {
		char tmpch = *start;

		*start++ = *--end;
		*end = tmpch;
	}

	return;
}


static char *
itoa(intmax_t num,
     char *str,
     unsigned int base)
{
	int i = 0;
	bool isNegative = false;

	if (base < 2 || base > 36) {
		*str = '\0';

		return str;
	}

	/* Handle 0 explicitly, otherwise an empty string is printed for 0 */
	if (num == 0) {
		str[i++] = '0';
		str[i] = '\0';

		return str;

	}
	if (num < 0 && base == 10) {
		isNegative = true;
		num = -num;
	}
	while (num != 0) {
		unsigned int rem = (unsigned int) (num % base);

		str[i++] = (char) ((rem > 9) ? (rem-10) + 'a' : rem + '0');
		num = num / base;
	}
	if (isNegative) {
		str[i++] = '-';
	}
	str[i] = '\0';
	strreverse(str);

	return str;
}

static char *
itoa2(intmax_t value,
      char *result,
      unsigned int base)
{
	char *ptr = result;
	char *ptr1 = result;
	char tmp_char;
	intmax_t tmp_value;

	if (base < 2 || base > 36) {
		*result = '\0';

		return result;
	}

#if 0
	/* xxx a possible optimization */
	if (value == 0) {
		unsigned int i =  0;

		result[i++] = '0';
		result[i] = '\0';

		return result;

	}
#endif
	do {
		tmp_value = value;
		value /= base;
		*ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + (tmp_value - value * base)];
	} while (value);

	// Apply negative sign
	if (tmp_value < 0) {
		*ptr++ = '-';
	}
	*ptr-- = '\0';
	while (ptr1 < ptr) {
		tmp_char = *ptr;
		*ptr--= *ptr1;
		*ptr1++ = tmp_char;
	}
	return result;
}


static char *
binary_fmt(uintmax_t x,
           int zf)			/* natural promoted type of bool */
{
	static char str[(sizeof(x) * CHAR_BIT) + 1];
	char *s = str + sizeof(str) - 1;

	*str = *s = '\0';
	do {
#if 0	/* oh wonderful compiler, which is best? */
		*--s = (char) ('0' + (x % 2));
#else
# if (defined(__STDC_VERSION__) &&                                     \
      ((__STDC_VERSION__ - 0) >= 201112L))	/* i.e. supports Universal Character Names */
		*--s = (char) (U'0' + (x & 1));
# else
		*--s = (char) ('0' + (x & 1));
# endif
#endif
#if 0	/* oh wonderful compiler, which is best? */
		x /= 2;
#else
		x >>= 1;
#endif
	} while (x);

	if (zf) {
		while (s > str)
			*--s = '0';
	}

	return s;
}

int
main(void)
{
	intmax_t n = 1567;
	char str[100];

	memset(str, '\0', sizeof(str));
	itoa(n, str, 10);
	printf("Base:10 %s\n", str);
	memset(str, '\0', sizeof(str));
	itoa2(n, str, 10);
	printf("Base:10 %s\n", str);

	memset(str, '\0', sizeof(str));
	itoa(-n, str, 10);
	printf("Base:10 %s\n", str);
	memset(str, '\0', sizeof(str));
	itoa2(-n, str, 10);
	printf("Base:10 %s\n", str);

	memset(str, '\0', sizeof(str));
	itoa(n, str, 2);
	printf("Base:2  %s\n", str);
	memset(str, '\0', sizeof(str));
	itoa2(n, str, 2);
	printf("Base:2  %s\n", str);
	printf("Base:2  %s\n", binary_fmt((uintmax_t) n, false));
	printf("Base:2  %s\n", binary_fmt((uintmax_t) n, true));

	memset(str, '\0', sizeof(str));
	itoa(n, str, 8);
	printf("Base:8  %s\n", str);
	memset(str, '\0', sizeof(str));
	itoa2(n, str, 8);
	printf("Base:8  %s\n", str);

	memset(str, '\0', sizeof(str));
	itoa(n, str, 16);
	printf("Base:16 %s\n", str);
	memset(str, '\0', sizeof(str));
	itoa2(n, str, 16);
	printf("Base:16 %s\n", str);

	memset(str, '\0', sizeof(str));
	itoa(n, str, 35);
	printf("Base:35 %s\n", str);
	memset(str, '\0', sizeof(str));
	itoa2(n, str, 35);
	printf("Base:35 %s\n", str);

	memset(str, '\0', sizeof(str));
	itoa(0UL, str, 10);
	printf("Base:10 %s\n", str);
	memset(str, '\0', sizeof(str));
	itoa2(0UL, str, 10);
	printf("Base:10 %s\n", str);

	exit(0);
}


/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " LDLIBS=-lm " fn " && ./" fn))
 * End:
 */
