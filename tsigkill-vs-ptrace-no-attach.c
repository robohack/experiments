#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <strings.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ptrace.h>

static pid_t kid;

static void trcwait(int *sp)
{
 pid_t wrv;
 int s;

 wrv = wait4(kid,&s,0,0);
 if (wrv < 0)
  { printf("parent: wait error: %s\n",strerror(errno));
    exit(1);
  }
 if (wrv == 0)
  { printf("parent: wait4 returned 0\n");
    exit(1);
  }
 if (wrv != kid)
  { printf("parent: wait4 returned unexpected pid %d\n",(int)wrv);
    exit(1);
  }
 if (! WIFSTOPPED(s))
  { printf("parent: wait shows non-stopped status %#x\n",s);
    exit(1);
  }
 printf("parent: wait shows signal %d\n",WSTOPSIG(s));
 *sp = s;
}

static void trccont(int sig)
{
 printf("parent: continuing child with signal %d\n",sig);
 if (ptrace(PT_CONTINUE,kid,(caddr_t)1,sig) < 0)
  { printf("parent: PT_CONTINUE: %s\n",strerror(errno));
    exit(1);
  }
}

int main(void);
int main(void)
{
 int p[2];
 char junk;
 int status;

 /* We need this pipe to ensure the parent doesn't signal the child
    until the child has done the PT_TRACE_ME.  Normal debugging doesn't
    need this because the child execs, and an exec by a traced process
    produces an automatic stop.  We don't exec, so we have to provoke a
    stop somehow...but sending the signal before the TRACE_ME request
    will either kill the process or do nothing, depending on the action
    for the signal in question.  Hence this pipe. */
 if (pipe(p) < 0)
  { printf("pipe: %s\n",strerror(errno));
    exit(1);
  }
 kid = fork();
 if (kid < 0)
  { printf("fork: %s\n",strerror(errno));
    exit(1);
  }
 if (kid == 0)
  { printf("child: pid is %d\n",(int)getpid());
    if (ptrace(PT_TRACE_ME,0,0,0) < 0)
     { printf("child: PT_TRACE_ME: %s\n",strerror(errno));
       exit(1);
     }
    close(p[0]);
    close(p[1]);
    while (1)
     { printf("child: tick\n");
       sleep(5);
     }
  }
 printf("parent: child pid is %d\n",(int)kid);
 close(p[1]);
 read(p[0],&junk,1);
 printf("parent: sending SIGINT to provoke stop\n");
 kill(kid,SIGINT);
 trcwait(&status);
 trccont(0);
 printf("parent: sleeping\n");
 sleep(12);
 printf("parent: sending SIGKILL\n");
 kill(kid,SIGKILL);
 trcwait(&status);
 trccont(0);
 printf("parent: sleeping\n");
 sleep(12);
 printf("parent: shutting down\n");
 kill(kid,SIGKILL);
 trcwait(&status);
 ptrace(PT_KILL,kid,0,0);
 exit(0);
}
