#include <stdio.h>
#include <stdlib.h>

enum {
	SZ = 100,
};

int
main(void)
{
	int i = 0;
	int input;
	char array[SZ + 1];

	while ((input = getchar()) != EOF) {
		if (input == '\n' || i >= SZ) {
			array[i] = '\0';
			printf("%s\n", array);
			i = 0;
		} else {
			array[i] = (char) input;
			i += 1;
		}
	}
	exit(0);
}
