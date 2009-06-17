/*
 * NAME: Brian Buhrow
 * DATE: June 9, 2009
 *
 * PURPOSE: The purpose of this test program is to see if we can figure out
 * why ptys don't seem to work right under NetBSD-5.x.  There seems to be some
 * sort of deadlock issue between the pty master and the slave under certain
 * conditions, where the pty gets data between the master and the slave, and
 * each is waiting for someting to happen.
 *
 * The master is waiting for a write(2) to complete, and the slave is waiting
 * for read(2) to complete.
 *
 * This works fine under NetBSD-4.x and earlier, but NetBSD-5 seems to have a
 * problem.
 *
 * This is a test program which should easily reproduce the problem.
 */

#ifndef LINT
static char rcsid[] = "$Id$";
#endif /*LINT*/

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <util.h>

/* Slave reading process */

int
slave(int slavefd)
{
	char buf[512], *ptr;
	int bytesread;
	pid_t slpid;

	slpid = getpid();

	while (1) {
		bytesread = read(slavefd, buf, sizeof(buf));
		printf("%d: Read %d bytes from pty\n",slpid,bytesread);
		if (bytesread < 0) {
			perror("Error eading from pty");
			exit(1);
		}
	}

	exit(0);
	/* NOT REACHED */
}

/* Master writing process */
int
master(int masterfd)
{
	char buf[512], *ptr;
	int outbytes,i;
	pid_t curpid;

	curpid = getpid();

	while (1) {
		sprintf(buf, "q {Subject: June Monitor}\rd\r");
		for (i = 0; i < 11; i ++) {
			outbytes = write(masterfd, buf, strlen(buf));
			ptr = buf;
			strncat(buf, ptr, 24);
			printf("%d: Wrote %d bytes to master pty\n", curpid, outbytes);
			if (outbytes < 0) {
				perror("write");
			}
		}
		bzero(buf, sizeof(buf));
		sleep(5);
	}

	exit(0);
	/* NOT REACHED */
}
		

int
main(int argc, char **argv) 
{
	pid_t child;
	int masterfd, slavefd, status;
	char ptyname[PATH_MAX];

	status = openpty(&masterfd, &slavefd, ptyname, NULL, NULL);
	if (status < 0) {
		perror("Openpty");
		exit(1);
	}
	child = fork();
	if (child < 0) {
		perror("fork");
		exit(1);
	}
	if (child) {
		printf("%s: Master process(%d) is writing to slave process (%d)\n",
		       argv[0], getpid(), child);
		printf("%s: Using pty %s\n", argv[0], ptyname);
		master(masterfd);
	} else {
		slave(slavefd);
	}

	exit(0);
	/* NOT REACHED */
}
