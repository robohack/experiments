/*
** TEST_FLOCK -- Diagnostic for flock() function.
**
** Useage: test_flock filename
**
** Where "filename" is any scratch file that exists and has r/w
** permissions for user.  Use filename shorter than 450 characters.
** 
** Some unices require that a file be opened for write before
** an flock() call can succeed.  This program tests for that
** condition, producing exit status and diagnostic messages
** according to what it finds.
**
** For a typical system where lock requires write, such as
** a.out Linux 2.0.29 kernel with libc-4.7.6, if you run this
** program with a file argument with read/write permissions:
**
** ls -l nope
** -rw-rw-r--   1 dan      users           0 Jul  2 23:51 nope
** test_flock nope
** Can't lock file nope -- flock requires write: Bad file number
** 
** Exit status is 1 (failure).  I don't know why perror() returns
** "Bad file number".
**
** On the other hand, where flock does not require write, as with
** Linux and libc-5.3.12, no output is produced, and exit status
** is 0 (success).
**
** Copyright 1997 Dan Wilder.  Published under the terms and
** conditions given in the Smail General Public License.
*/

#include <sys/file.h>

char buf[512];

int 
open_it(file, mode, pa)
char *file;
int mode;
char *pa;
{
    int fd;
    fd = open(file, mode);
    if (fd == -1)
        {
        strcpy(buf, "Can't open file ");
        strcat(buf, file);
        strcat(buf, " ");
        strcat(buf, pa);
        perror(buf);
        exit(1);
        }
    return fd;
}
  
int 
lock_it(fd, file, pa)
int fd;
char *file;
char *pa;
{
    int rc;

    rc = flock(fd, LOCK_EX | LOCK_NB);
    if (rc == -1)
        {
        strcpy(buf, "Can't lock file ");
        strcat(buf, file);
        strcat(buf, " ");
        strcat(buf, pa);
        perror(buf);
        exit(1);
        }
    rc = flock(fd, LOCK_UN);
    if (rc == -1)
        {
        strcpy(buf, "Can't unlock file ");
        strcat(buf, file);
        strcat(buf, pa);
        strcat(buf, " ");
        perror(buf);
        exit(1);
        }
}

void 
close_it(fd, file)
int fd;
char *file;
{
    int rc;

    rc = close(fd);
    if (rc == -1)
        {
        strcpy(buf, "Can't close file ");
        strcat(buf, file);
        perror(buf);
        exit(1);
        }
}

main(argc, argv)
int argc;
char **argv;
{
    int fd;
    char *pa;

    if (argc != 2 || strlen(argv[1]) > 450)
        {
        printf("Useage: %s file\n", *argv);
        printf("file must exist and have read/write permissions\n");
        exit(1);
        }

    /*
    ** First try to open read/write; if can't flock in that
    ** condition, more is wrong than I know about.
    **
    ** If they detect failure, these functions print error
    ** messages and exit the program, with failure exit status.
    */
    pa = argv[1];
    fd = open_it(pa, O_RDWR, "read/write");
    lock_it(fd, pa, " -- flock is badly broken");
    close_it(fd, pa);

    /*
    ** flock worked for file open read/write; now try
    ** read-only, which should succeed on most unices.
    */
    fd = open_it(pa, O_RDONLY, "read-only");
    lock_it(fd, pa, " -- flock requires write");
    close_it(fd, pa);

    /*
    ** It all worked, exit with no messages and success code.
    */
    exit(0);
}
