#include <stdio.h>
#include <stdlib.h>

#ifdef __NetBSD__

#include <errno.h>
#include <stdbool.h>

/*
 * keeping malloc et al out can shrink the program significantly:
 *
 *    text    data     bss     dec     hex filename
 *  526228   10184 2177529 2713941  296955 thello
 *
 *    text    data     bss     dec     hex filename
 *  129119    3480   16912  149511   24807 thello
 */
void *malloc(size_t n) { (void) n; return NULL; }
void *realloc(void *p, size_t n) { (void) p; (void) n; return NULL; }
void *calloc(size_t n, size_t sz) { (void) n; (void) sz; return NULL; }
void free(void *p) { (void) p; }

/*
 * Adding the rest of the junk shrinks things a bit more:
 *
 *    text    data     bss     dec     hex filename
 *  121450    3384   15520  140354   22442 thello
 */

/* this program doesn't call atexit(), so avoid it */
void __cxa_finalize() {}
void __cxa_atexit() {}
void __libc_atexit_init() {}
void __cxa_thread_run_atexit() {}
bool __cxa_thread_atexit_used = 0;
int atexit(void (*function)(void)) { (void) function; return 0; }

/* atomics */
void __libc_atomic_init() {}

/* getenv() infrastructure */
char *__allocenvvar() { return 0; }
bool __canoverwriteenvvar() { return true; }
size_t __envvarnamelen() { return 0; }
void *__findenv() { return 0; }
void *__findenvvar() { return 0; }
void __freeenvvar() { }
ssize_t __getenvslot() { return 0; }
void __libc_env_init() { }
bool __readlockenv() { return true; }
bool __unlockenv() { return true; }
bool __writelockenv() { return false; }

/* Thread Local Storage */
void __libc_rtld_tls_allocate() { }
void __libc_rtld_tls_free() { }
void __libc_static_tls_setup() { }
void __libc_tls_get_addr() { }

/* stack protector */
void __chk_fail() { }
void __guard_setup() { }
void __stack_chk_fail() { }
void __stack_chk_fail_local() { }
#if 0
long __stack_chk_guard[8] = {0};
#else
int __stack_chk_guard = 0;
#endif

/* pthreads */
void	__libc_thr_init(void) {}

/* errno */
int *__errno(void) { return &errno; }

#endif	/* __NetBSD__ */

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
