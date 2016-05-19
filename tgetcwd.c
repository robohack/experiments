#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void);

int
main()
{
	char current_dir[BUFSIZ];

	printf("Working directory: %s\n", getcwd(current_dir, sizeof(current_dir)));

	exit(0);
}
