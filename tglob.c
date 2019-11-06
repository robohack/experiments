#include <errno.h>
#include <fcntl.h>
#include <glob.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(void);

static char *
makefile(const char *dn,
	 const char *fn)
{
	char *pn = NULL;

	asprintf(&pn, "%s/%s", dn, fn);
	close(open(pn, O_CREAT));

	return pn;
}

/*
 * NetBSD v.s. GlibC:  glob(3) of files in a symlink to a directory
 */
static bool
test_in_symlink_to_dir(void)
{
	char dfn[128] = "";
	char lfn[128] = "";
	char *fn1;
	char *fn2;
	int ret;
	size_t i;
	glob_t pglob;

	strlcpy(dfn, "/tmp/testsymlink_dir.XXXXXX", sizeof(dfn));
	if (mkdtemp(dfn) == NULL) {
		fprintf(stderr, "%s: ERROR: mkdtemp(%s) failed: %s\n", __func__, dfn, strerror(errno));
		return true;
	}
	fn1 = makefile(dfn, "foo.1");
	fn2 = makefile(dfn, "foo.2");

	strlcpy(lfn, "/tmp/testsymlink_lnk.XXXXXX", sizeof(lfn));
	mktemp(lfn);
	symlink(dfn, lfn);
#if 0
	/*
	 * On NetBSD this does work, but CVS does not chdir() to the module
	 * directory:
	 */
	chdir(lfn);
	ret = glob("*.*", 0, 0, &pglob);
#else
        /*
	 * This is effectively what CVS does to find "*,v" files in modules, and
	 * if a module directory is a symlink then this should still work....
	 *
	 * XXX However on NetBSD this does not work!
	 *
	 * But it does work with:
	 *
	 * - GlibC/gnulib (i.e. on linux, and with the glob() included in CVS sources)
	 * - macos (at least 10.13.x)
	 * - FreeBSD-12.0
	 * - probably OpenBSD (based on inspection of code)
	 */
	ret = glob("/tmp/testsymlink_lnk.*/*.*", 0, 0, &pglob);
#endif
	if (ret != 0) {
		fprintf(stderr, "%s: ERROR: glob() failed: %d: %s\n", __func__, ret, strerror(errno));
	}
	if (pglob.gl_pathc != 2) {
		fprintf(stderr, "%s: ERROR: pglob.gl_pathc = %ju, but expected 2!\n", __func__, (uintmax_t) pglob.gl_pathc);
	}
	for (i = 0; i < pglob.gl_pathc; i++) {
		printf("%s: pglob.gl_pathv[%ju] = \"%s\"\n", __func__, (uintmax_t) i, pglob.gl_pathv[i]);
	}
	unlink(fn1);
	free(fn1);
	unlink(fn2);
	free(fn2);
	unlink(lfn);
	rmdir(dfn);

	return (pglob.gl_pathc != 2);
}

/*
 * NetBSD v.s. GlibC:  for fun also test https://sourceware.org/bugzilla/show_bug.cgi?id=866
 */
static bool
test_dangling_symlink_matching(void)
{
	char lfn1[128] = "";
	char lfn2[128] = "";
	int ret;
	size_t i;
	glob_t pglob;

	strlcpy(lfn1, "/tmp/testsymlink.XXXXXX", sizeof(lfn1));
	mktemp(lfn1);
	symlink("/no_such_file_exists", lfn1);

	strlcpy(lfn2, "/tmp/testsymlink.XXXXXX", sizeof(lfn2));
	mktemp(lfn2);
	symlink("/usr", lfn2);          /* an existing file! */

	chdir("/tmp");
	ret = glob("testsymlink.*", 0, 0, &pglob);
	if (ret != 0) {
		fprintf(stderr, "%s: ERROR: glob() failed: %d: %s\n", __func__, ret, strerror(errno));
	}
	if (pglob.gl_pathc != 2) {
		fprintf(stderr, "%s: ERROR: pglob.gl_pathc = %ju, but expected 2!\n", __func__, (uintmax_t) pglob.gl_pathc);
	}
	for (i = 0; i < pglob.gl_pathc; i++) {
		printf("%s: pglob.gl_pathv[%ju] = \"%s\"\n", __func__, (uintmax_t) i, pglob.gl_pathv[i]);
	}
	unlink(lfn1);
	unlink(lfn2);

	return (pglob.gl_pathc != 2);
}

int
main()
{
	bool failed = false;

	failed |= test_in_symlink_to_dir();
	failed |= test_dangling_symlink_matching();

	exit(failed);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
