#include <stdio.h>
#include <signal.h>
main()
{
	char signm[SIG2STR_MAX];
	int signum;

	printf("strsignal(11) = %s\n", strsignal(11));
	printf("strsignal(28) = %s\n", strsignal(28));

	str2sig("SEGV", &signum);
	printf("str2sig(SEGV) = %d\n", signum);
	str2sig("VTALRM", &signum);
	printf("str2sig(VTALRM) = %d\n", signum);
	str2sig("RTMIN", &signum);
	printf("str2sig(RTMIN) = %d\n", signum);
	str2sig("RTMIN+0", &signum);
	printf("str2sig(RTMIN+0) = %d\n", signum);
	str2sig("RTMIN+1", &signum);
	printf("str2sig(RTMIN+1) = %d\n", signum);
	str2sig("RTMAX-1", &signum);
	printf("str2sig(RTMAX-1) = %d\n", signum);
	str2sig("RTMAX", &signum);
	printf("str2sig(RTMAX) = %d\n", signum);

	if (sig2str(11, signm) == -1)
		sprintf(signm, "#%d", 11);

	printf("sig2str(11) = %s\n", signm);
	if (sig2str(28, signm) == -1)
		sprintf(signm, "#%d", 28);

	printf("sig2str(28) = %s\n", signm);
	if (sig2str(SIGRTMIN, signm) == -1) {
		sprintf(signm, "#%d", SIGRTMIN);
	}
	printf("sig2str(SIGRTMIN) = %s\n", signm);
	if (sig2str(SIGRTMIN+1, signm) == -1) {
		sprintf(signm, "#%d", SIGRTMIN+1);
	}
	printf("sig2str(SIGRTMIN+1) = %s\n", signm);
	if (sig2str(SIGRTMAX-1, signm) == -1) {
		sprintf(signm, "#%d", SIGRTMAX-1);
	}
	printf("sig2str(SIGRTMAX-1) = %s\n", signm);
	if (sig2str(SIGRTMAX, signm) == -1) {
		sprintf(signm, "#%d", SIGRTMAX);
	}
	printf("sig2str(SIGRTMAX) = %s\n", signm);

	exit(0);
}
