/*
Date: Sat, 15 Aug 2020 07:24:01 -0400 (EDT)
From: Mouse <mouse@Rodents-Montreal.ORG>
Message-Id: <202008151124.HAA26856@Stone.Rodents-Montreal.ORG>
To: tech-kern@NetBSD.org
Subject: Re: wait(2) and SIGCHLD
 */
/* reformatted -- gaw */

#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <sys/wait.h>

int main(void);

int
main(void)
{
	pid_t kid;
	pid_t dead;

	fflush(0);
	kid = fork();
	if (kid == 0) {
		printf("child sleeping\n");
		sleep(5);
		printf("child exiting\n");
		exit(0);
	}
	printf("child %d forked\n",(int)kid);
	printf("parent sleeping\n");
	sleep(1);
	printf("parent ignoring SIGCHLD\n");
	signal(SIGCHLD,SIG_IGN);
	while (1) {
		printf("parent waiting\n");
		dead = wait(0);
		if (dead < 0) {
			if (errno == ECHILD) {
				printf("wait error: %s\n",strerror(ECHILD));
				break;
			} else {
				printf("wait error: %s\n",strerror(errno));
			}
		} else {
			printf("child %d reaped\n",dead);
		}
	}
	printf("parent exiting\n");

	exit(0);
}
