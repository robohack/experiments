#include <stdlib.h>

/*
 * string_reverse1() -- reverse a string "in place".
 * 
 * Assume 'string' points to a valid, writable, NUL-terminated string.
 */
void
string_reverse1(char *string) 
{
	char *end = string;
 
	while (*end != '\0') {
		end++;
	}
	/* note 'end' now points past the end of the string, at the NUL */

	while (string < end) {
		char tmpch = *string;

		*string++ = *--end;
		*end = tmpch;
	}

	return;
}


/*
 * string_reverse2() -- return a reversed copy of a string.
 * 
 * Assume 'string' points to a valid, writable, NUL-terminated string.
 *
 * Works without modifying the original.
 *
 * Returned value is a pointer to new storage that may be released when it is
 * no longer needed by passing it to free(2).
 *
 * Returns NULL if enough new storage cannot be allocated and with errno set as
 * per malloc(3).
 */
char *
string_reverse2(const char *string)
{
	size_t len = 0;
	char *end = string;
	char *new;
	char *newp;
 
	while (*end != '\0') {
		len++;
		end++;
	}
	/* note 'end' now points past the end of the string, at the NUL */

	if ((newp = new = malloc(len)) == NULL) {
		return NULL;
	}
	
	while (end > string) {
		*newp++ = *--end;
	}

	return new;
}
