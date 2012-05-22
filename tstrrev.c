#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <vis.h>
 
void strreverse(char *);
void sentence_rev(char *);

/*
 * string_reverse1() -- reverse a string "in place".
 * 
 * Assume 'start' points to a valid, writable, NUL-terminated string.
 */
void
string_reverse1(char *string) 
{
	char *end = string;
 
	while (*end != '\0')
		end++;
	/* note 'end' now points past the end of the string, at the NUL */
	while (string < end) {
		char tmpch = *string;

		*string++ = *--end;
		*end = tmpch;
	}

	return;
}


char *
string_reverse2(const char *string)
{
/* your code here */
}
