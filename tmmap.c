/* $Id: mmap_tests.c,v 1.1 1996/08/10 20:12:09 proff Exp $
 * 
 * various mmap() immplimentations suck, we attempt to find out just how
 * hard.
 *
 *        - Julian Assange (proff@suburbia.net)
 *
 * Test results: (please send additions to proff@suburbia.net)
 *
 *   Linux suburbia 2.0.0 #29- Thu Jul 11 18:03:20 EST 1996 i586 
 *
 *	HAVE_MMAP_FILE_PRIVATE_READ
 *	HAVE_MMAP_FILE_PRIVATE_CHILD_INHERIT
 *	HAVE_MMAP_FILE_PRIVATE_WRITE
 *	HAVE_MMAP_FILE_SHARED_READ
 *	HAVE_MMAP_FILE_SHARED_CHILD_INHERIT
 *	HAVE_MMAP_FILE_SHARED_CHILD_READ_PARENT_WRITE
 *	HAVE_MMAP_FILE_SHARED_PARENT_READ_CHILD_WRITE
 *	HAVE_MMAP_FILE_SHARED_WRITE
 *	HAVE_MMAP_DEV_ZERO_PRIVATE
 *	HAVE_MMAP_DEV_ZERO_PRIVATE_CHILD_INHERIT
 *	HAVE_MMAP_ANON_PRIVATE
 *	HAVE_MMAP_ANON_PRIVATE_CHILD_INHERIT
 *
 *   FreeBSD profane 2.2-CURRENT #0 Sat Jul 27 19:16:00 EST 1996
 *
 *	HAVE_MMAP_FILE_PRIVATE_READ
 *	HAVE_MMAP_FILE_PRIVATE_CHILD_INHERIT
 *	HAVE_MMAP_FILE_PRIVATE_WRITE
 *	HAVE_MMAP_FILE_SHARED_READ
 *	HAVE_MMAP_FILE_SHARED_CHILD_INHERIT
 *	HAVE_MMAP_FILE_SHARED_CHILD_READ_PARENT_WRITE
 *	HAVE_MMAP_FILE_SHARED_PARENT_READ_CHILD_WRITE
 *	HAVE_MMAP_FILE_SHARED_WRITE
 *	HAVE_MMAP_DEV_ZERO_PRIVATE
 *	HAVE_MMAP_DEV_ZERO_PRIVATE_CHILD_INHERIT
 *	HAVE_MMAP_DEV_ZERO_SHARED
 *	HAVE_MMAP_DEV_ZERO_SHARED_CHILD_INHERIT
 *	HAVE_MMAP_DEV_ZERO_SHARED_CHILD_READ_PARENT_WRITE
 *	HAVE_MMAP_DEV_ZERO_SHARED_PARENT_READ_CHILD_WRITE
 *	HAVE_MMAP_ANON_PRIVATE
 *	HAVE_MMAP_ANON_PRIVATE_CHILD_INHERIT
 *	HAVE_MMAP_ANON_SHARED
 *	HAVE_MMAP_ANON_SHARED_CHILD_INHERIT
 *	HAVE_MMAP_ANON_SHARED_CHILD_READ_PARENT_WRITE
 *	HAVE_MMAP_ANON_SHARED_PARENT_READ_CHILD_WRITE
 */

#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define MM_SIZE (100*1024)
#define TEST_FILE "mmap.test"

int caught_sigint;

void
sigint(sig)
	int sig;
{
	caught_sigint++;
	signal (SIGINT, sigint);
}

/* 
 * TODO: test MAP_INHERIT, MAP_FIXED  (can't see that latter being much of an issue)
 */

void
test_child(p, msg)
	char *p;
	char *msg;
{
	char *im = "inherit_magic";
	char *pm = "parent_magic";
	char *cm = "child_magic";
	pid_t pid;
	strcpy (p, im);
	caught_sigint = 0;
	signal (SIGINT, sigint);
	fflush (stdout);
	pid = fork();
	if (pid<0)
		return;
	if (pid==0)
	{
		if (strcmp(p, im)==0)
			printf("%s_CHILD_INHERIT\n", msg);
		kill (getppid(), SIGINT);
		while (!caught_sigint)
			pause ();
		caught_sigint = 0;
		if (strcmp(p, pm)==0)
			printf("%s_CHILD_READ_PARENT_WRITE\n", msg);
		strcpy (p, cm);
		fflush (stdout);
		kill (getppid(), SIGINT);
		exit(0);
	}
	/* parent */
	while (!caught_sigint)
		pause ();
	caught_sigint = 0;
	strcpy (p, pm);
	kill (pid, SIGINT);
	while (!caught_sigint)
		pause ();
	if (strcmp(p, cm)==0)
		printf("%s_PARENT_READ_CHILD_WRITE\n", msg);
	signal (SIGINT, SIG_DFL);
}

int
main(argc, argv)
	int argc;
	char *argv[];
{
	int fd;
	char *m = malloc(MM_SIZE);
	char buf[1024];
	signal (SIGSEGV, SIG_IGN); /* the strcmp's should still fail */
#if defined(MAP_PRIVATE)
	fd = open(TEST_FILE, O_RDWR|O_CREAT|O_TRUNC, 0666);
	strcpy (m, "mmap magic");
	write (fd, m, 11);
	lseek (fd, 0, SEEK_SET);
	if (fd>=0)
	{
		char *p=(char *)mmap(0, MM_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
		if (p!=(char *)-1)
		{
			if (strcmp(p, m)==0)
			 	puts("HAVE_MMAP_FILE_PRIVATE_READ");
			test_child(p, "HAVE_MMAP_FILE_PRIVATE");
			strcpy (p, "mmap magic2");
			munmap (p, MM_SIZE);
			read (fd, buf, 12);
			if (strcmp(buf, "mmap magic2")!=0) /* we shouldn't be writing to the file in private mode */
				puts("HAVE_MMAP_FILE_PRIVATE_WRITE");
		}
		close (fd);
	}
#endif
#if defined(MAP_SHARED) /* we should fork off another process to test the sharing */
	fd = open(TEST_FILE, O_RDWR|O_CREAT|O_TRUNC, 0666);
	strcpy (m, "mmap magic");
	write (fd, m, 11);
	lseek (fd, 0, SEEK_SET);
	if (fd>=0)
	{
		char *p=(char *)mmap(0, MM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		if (p!=(char *)-1)
		{
			if (strcmp(p, m)==0)
			 	puts("HAVE_MMAP_FILE_SHARED_READ");
			test_child(p, "HAVE_MMAP_FILE_SHARED");
			strcpy (p, "mmap magic2");
			munmap (p, MM_SIZE);
			read (fd, buf, 12);
			if (strcmp(buf, "mmap magic2")==0)
				puts("HAVE_MMAP_FILE_SHARED_WRITE");
		}
		close (fd);
	}
#endif
#if defined(MAP_PRIVATE)
	fd = open("/dev/zero", O_RDWR, 0666);
	if (fd>=0)
	{
		char *p=(char *)mmap(0, MM_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
		if (p!=(char *)-1)
		{
			strcpy (p, "mmap magic_dev_zero");
			if (strcmp (p, "mmap magic_dev_zero")==0)
			 	puts("HAVE_MMAP_DEV_ZERO_PRIVATE");
			test_child(p, "HAVE_MMAP_DEV_ZERO_PRIVATE");
			munmap (p, MM_SIZE);
		}
		close (fd);
	}
#endif
#if defined(MAP_PRIVATE)
	fd = open("/dev/zero", O_RDWR, 0666);
	if (fd>=0)
	{
		char *p=(char *)mmap(0, MM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		if (p!=(char *)-1)
		{
			strcpy (p, "mmap magic_dev_zero_shared");
			if (strcmp (p, "mmap magic_dev_zero_shared")==0)
			 	puts("HAVE_MMAP_DEV_ZERO_SHARED");
			test_child(p, "HAVE_MMAP_DEV_ZERO_SHARED");
			munmap (p, MM_SIZE);
		}
		close (fd);
	}
#endif
#if defined(MAP_ANONYMOUS) && !defined(MAP_ANON)
#define MAP_ANON MAP_ANONYMOUS
#endif
#ifdef MAP_ANON
	{
		char *p=(char *)mmap(0, MM_SIZE, PROT_READ | PROT_WRITE, MAP_ANON|MAP_PRIVATE, -1, 0);
		if (p!=(char *)-1)
		{
			strcpy (p, "mmap magic_anon");
			if (strcmp(p, "mmap magic_anon")==0)
			 	puts("HAVE_MMAP_ANON_PRIVATE");
			test_child(p, "HAVE_MMAP_ANON_PRIVATE");
			munmap (p, MM_SIZE);
		}
	}
#endif
#if defined(MAP_ANON) && defined(MAP_SHARED) /* we should fork off another process to test the sharing */
	{
		char *p=(char *)mmap(0, MM_SIZE, PROT_READ | PROT_WRITE, MAP_ANON|MAP_SHARED, -1, 0);
		if (p!=(char *)-1)
		{
			strcpy (p, "mmap magic_shared_anon");
			if (strcmp(p, "mmap magic_shared_anon")==0)
			 	puts("HAVE_MMAP_ANON_SHARED");
			test_child(p, "HAVE_MMAP_ANON_SHARED");
			munmap (p, MM_SIZE);
		}
	}
#endif
	unlink(TEST_FILE);
	exit (0);
}
