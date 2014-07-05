#! /bin/awk -f

function random_ascii_string(slen)
{
	rc = 0;
	for (i = 0; i < slen; i++) {
		rc = int(rand() * (127 - 32)) + 32;
		if (rc == 39 || rch == 92) {
			i--;
			continue;
		}
		if (DEBUG > 0) {
			printf("%s%3d=%c", i==0 ? "" : ", ", rc, rc);
		} else {
			printf("%c", rc);
		}
		if (rc < 32 || rc > 126) {
			printf("Houston, we have a problem!\n");
			exit;
		}
	}
	printf("\n");
}

BEGIN {
	if (DEBUG > 0) {
		printf("RSEED='%s', LEN='%s', NUM='%s', DEBUG='%s'\n", RSEED, LEN, NUM, DEBUG);
	}
	if (RSEED == 0 && RSEED == "") {
		srand();
		RSEED = srand();
	}
	oseed = srand(RSEED);
	if (LEN == 0 || LEN == "") {
		LEN = 11;
	}
	if (NUM == 0 || NUM == "") {
		NUM = 1000;
	}
	for (s = 0; s < NUM; s++) {
		random_ascii_string(LEN);
	}
}

# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (concat "awk -v DEBUG=1 -v LEN= -v NUM= -v RSEED=${RANDOM} -f " (file-name-nondirectory (buffer-file-name)))
# End:
