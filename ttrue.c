#ifndef EXIT_VALUE
# define EXIT_VALUE 0
#endif

#ifdef __NetBSD__

/*
 * Initiative to make this actually work comes from Michael van Elst's post:
 *
 *	https://mail-index.netbsd.org/tech-userlevel/2023/07/01/msg013971.html
 */

# if 0

/*
 * xxx it seems we cannot nullify this, perhaps because as the comment says:
 *
 * _libc_init is called twice.  One call comes explicitly from crt0.o
 * (for newer versions) and the other is via global constructor handling.
 */
void	_libc_init(void) __attribute__((__constructor__, __used__));
void _libc_init(void) {}

# else

#include <stdbool.h>
#include <sys/types.h>
#include <errno.h>

void *malloc(size_t n) { (void) n; return 0; }
void *realloc(void *p, size_t n) { (void) p; (void) n; return 0; }
void *calloc(size_t n, size_t sz) { (void) n; (void) sz; return 0; }
void free(void *p) { (void) p; }

/* exit() */
void abort(void) {}
void _exit(int);
void exit(int n) { _exit(n); }

/* atomics */
void __libc_atomic_init() {}

/* atexit() */
void __cxa_finalize() {}
void __cxa_atexit() {}
void __libc_atexit_init() {}
void __cxa_thread_run_atexit() {}
bool __cxa_thread_atexit_used = 0;
int atexit(void (*function)(void)) { (void) function; return 0; }

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
void __fail() { }
void __chk_fail() { }
void __guard_setup() { }
void __stack_chk_fail() { }
void __stack_chk_fail_local() { }
#if 0
long __stack_chk_guard[8] = {0}
#else
int __stack_chk_guard = 0;
#endif

/* pthreads */
void	__libc_thr_init(void) {}

/* errno */
int *__errno(void) { return &errno; }

# endif	 /* can't avoid _libc_init() */

#endif	/* __NetBSD__ */

/*
 * direct use of _exit() vs. return through main doesn't seem to make any
 * difference once we avoid atexit(), exit(), et al.
 */
#ifdef NO_RETURN_MAIN

void _exit(int);

void main(void);

__attribute__((__noreturn__)) void main() { _exit(0); }

#else

int main(void);
int main() {return 0;}

#endif

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; cc -g -Og -static -Wl,-Map=" fn ".map -o " fn " " (buffer-file-name) " && ./" fn))
 * End:
 */
