#include <sys/cdefs.h>

#ifdef __linux__
# define _BSD_SOURCE			/* for ECHOKE */
#endif
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

int main(void);

#ifndef TCSASOFT
# define TCSASOFT	0		/* for some non-BSD */
#endif
#ifndef CIGNORE
# define CIGNORE	0		/* for some BSDs */
#endif

int
main()
{
	ssize_t ret;
	unsigned char ch;
	struct termios tio;
	struct termios otio;

	if (tcgetattr(STDIN_FILENO, &tio) < 0) {
		err(1, "tcgetattr(STDIN_FILENO)");
	}
	otio = tio;

	/* turn off canonical processing and all echoing */
	tio.c_lflag &= ~(ICANON | ECHOKE | ECHOE | ECHO | ECHONL | CIGNORE);

	/* wait for one character to be available */
	tio.c_cc[VMIN] = 1;
	tio.c_cc[VTIME] = 0;

	if (tcsetattr(STDIN_FILENO, TCSANOW | TCSASOFT, &tio) < 0) {
		/* hope for the best.... */
		(void) tcsetattr(STDIN_FILENO, TCSANOW | TCSASOFT, &otio);
		err(1, "tcsetattr(STDIN_FILENO, TCSANOW | TCSASOFT...)");
	}
	setbuf(stdout, (char *) NULL);

	printf("Type a single character, and see its value:  ");

	switch ((ret = read(STDIN_FILENO, &ch, 1))) {
	case -1:
		warn("read(STDIN_FILENO, &ch, 1) returned an error");
		ch = (unsigned char) EOF;
		break;
	case 0:
		warnx("read(STDIN_FILENO, &ch, 1) returned nothing");
		ch = 0;
		break;
	case 1:
#if 0
		switch ((ret = write(STDOUT_FILENO, &ch, 1))) {
		case -1:
			warn("write(STDOUT_FILENO, &ch, 1) returned an error");
			break;
		case 0:
			warnx("write(STDOUT_FILENO, &ch, 1) returned zero");
			break;
		case 1:
			/* all is well! */
			break;
		default:
			warnx("write(STDOUT_FILENO, &ch, 1) returned too much! (%d)", ret);
			break;
		}
#else
		printf("0x%02X, aka 0%o\n", (unsigned int) ch, (unsigned int) ch);
#endif
		break;
	default:
		warnx("read(STDIN_FILENO, &ch, 1) returned too much! (%d)", ret);
		break;
	}

	if (tcsetattr(STDIN_FILENO, TCSANOW | TCSASOFT, &otio) < 0) {
		err(1, "tcsetattr(STDIN_FILENO, TCSANOW | TCSASOFT, &otio)");
	}

	exit(0);
}
