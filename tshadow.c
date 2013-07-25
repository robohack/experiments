#include <stdio.h>
#include <stdlib.h>

void
shadow_test()
{
	int i = 1;
	int tmp;

	{
		int j = i;
		float i = 100.0;

		printf("j = %d\n", j);

		tmp = i;
	}
	i = tmp;
	printf("i = %d\n", i);
}

int
main()
{
	shadow_test();

	exit(0);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
