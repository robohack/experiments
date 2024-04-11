#include <stdio.h>
#include <stdlib.h>

int main(void);

int
main()
{
	printf("Hello, %s!\n", "world");
	/*
	 * note:  The first is one of the shortest known English sentences using
	 * all of the letters of the alphabet (i.e. it's a closer-to-perfect
	 * panagram, as opposed to the slightly longer but more commonly known
	 * "The quick brown fox jumps over a lazy fox").  The foxy sentence was
	 * apparently developed by Western Union as a test for telex operators,
	 * though other sources suggest it can be dated back as far as 1888.
	 * This shorter one was apparently created by some anonymous scholar.
	 */
	fputs("Jackdaws love my big sphinx of quartz\n", stderr);
	fputs("Pack my box with five dozen liquor jugs\n", stderr);
	fputs("The quick brown fox jumps over a lazy dog\n", stderr);
	fputs("The quick brown fox jumps over lazy dogs\n", stderr);
	fputs("This Pangram contains four as, one b, two cs, one d, thirty es, six fs, five gs, seven hs, eleven is, one j, one k, two ls, two ms, eighteen ns, fifteen os, two ps, one q, five rs, twenty-seven ss, eighteen ts, two us, seven vs, eight ws, two xs, three ys, & one z.\n", stderr);

	exit(0);
}


/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
