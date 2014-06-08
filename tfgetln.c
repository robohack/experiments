#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int, char *[]);

int
main(argc, argv)
	int argc;
	char *argv[];
{
	FILE *fp;
	char *buf, *lbuf;
	size_t len;

	fp = fopen(argv[argc - 1], "r");
	if (! fp) {
		exit(1);
	}

	lbuf = NULL;
	while ((buf = fgetln(fp, &len))) {
		if (buf[len - 1] == '\n')
			buf[len - 1] = '\0';
		else {
			if ((lbuf = (char *) malloc(len + 1)) == NULL)
				err(1, NULL);
			memcpy(lbuf, buf, len);
			lbuf[len] = '\0';
			buf = lbuf;
		}
		printf("%s\n", buf);

		if (lbuf != NULL) {
			free(lbuf);
			lbuf = NULL;
		}
	}
	fclose(fp);
	exit(0);
}
