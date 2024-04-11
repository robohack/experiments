#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <util.h>
/*
 * https://stackoverflow.com/questions/23458160/final-output-on-slave-pty-is-lost-if-it-was-not-closed-in-parent-why
 *
 * hmmm...  works fine on NetBSD-5.x...
 */


int
main(void)
{
	int master, slave;
	char buf[BUFSIZ];
	size_t nread;

	openpty(&master, &slave, NULL, NULL, NULL);

	if (fork()) {			/* parent:*/
//		close(slave);		/* leave this out on FreeBSD and lose slave's final words ... WHY? */
		do {
			nread = read(master, buf, sizeof(buf));
			write(STDOUT_FILENO, buf, nread); /* echo child's output to stdout */
		} while (nread > 0);
	} else {			/* child: */
		login_tty(slave);	/* this makes child a session leader and slave a controlling
					 * terminal for it, then dup()s std{in,out,err} to slave */
		printf("Feeling OK :-)\n");
		sleep(1);
		printf("Feeling unwell? ... or maybe OK if you see this!\n"); /* this line may get lost*/
	}

	exit(0);
}


/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " LDLIBS='-lutil' " fn " && ./" fn))
 * End:
 */
