#include <glob.h>
#include <stdio.h>
#include <unistd.h>

int main(void);

/*
 * https://sourceware.org/bugzilla/show_bug.cgi?id=866
 */
void
test_dangling_symlink_matching()
{
	int i;
	glob_t pglob;

	chdir("/tmp");
	symlink("/nope", "foo1");
	symlink("/usr", "foo2");
	glob("foo[12]", 0, 0, &pglob);
	for (i = 0; i < pglob.gl_pathc; i++) {
		printf("pglob.gl_pathv[%d] = \"%s\"\n", i, pglob.gl_pathv[i]);
	}
	unlink("foo1");
	unlink("foo2");
}

/*
 * 
 */
void
test_in_symlink_to_dir()
{
	int i;
	glob_t pglob;

	chdir("/tmp");
	symlink("/nope", "foo1");
	symlink("/usr", "foo2");
	glob("foo[12]", 0, 0, &pglob);
	for (i = 0; i < pglob.gl_pathc; i++) {
		printf("pglob.gl_pathv[%d] = \"%s\"\n", i, pglob.gl_pathv[i]);
	}
	unlink("foo1");
	unlink("foo2");
}

int
main()
{
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
