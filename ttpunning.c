#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void);

/*
 * see also:
 * https://gustedt.wordpress.com/2016/08/17/effective-types-and-aliasing/
 */

int
main()
{
	char cbuf[BUFSIZ];
	char *cp = cbuf;

	strcpy(cbuf, "this is a character string");


	*cp++ = 'N';
	*((int *) cp) = ~0;
	cp += sizeof(int);		/* "((int *) cbuf)++;" is illegal! */
	*cp++ = 'Y';
	*((double *) cp) = 3.14159;
	cp += sizeof(double);		/* "((double *) cbuf)++;" is illegal! */

	puts(cbuf);

	exit(0);
}
