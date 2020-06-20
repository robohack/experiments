/*
 * hmmm.... how to avoid pulling in stdio???
 *
 * The following works, for stdio specifically, but is still rather large as it
 * includes the full C runtime startup with locale setup, thread local storage,
 * etc.
 */
void _exit(int);
void main(void);
__attribute__((__noreturn__)) void main() { _exit(1); }
