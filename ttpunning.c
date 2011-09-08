#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void);

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
