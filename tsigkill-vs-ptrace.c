#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <strings.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <sys/socket.h>

typedef struct kid KID;

struct kid {
  const char *tag;
  pid_t k;
  int p;
  } ;

static KID k1;
static KID k2;
static KID k3;
static int gopipe[2];

static void checked_fork(const char *tag, void (*kidmain)(int), KID *k)
{
 pid_t child;
 int comm[2];

 if (socketpair(AF_LOCAL,SOCK_STREAM,0,&comm[0]) < 0)
  { printf("pipe: %s\n",strerror(errno));
    exit(1);
  }
 child = fork();
 if (child < 0)
  { printf("fork: %s\n",strerror(errno));
    exit(1);
  }
 if (child == 0)
  { char junk;
    close(gopipe[1]);
    close(comm[0]);
    switch (read(gopipe[0],&junk,1))
     { case 1:
	  close(gopipe[0]);
	  (*kidmain)(comm[1]);
	  exit(0);
	  break;
       default:
	  exit(1);
	  break;
     }
  }
 close(comm[1]);
 k->tag = tag;
 k->p = comm[0];
 k->k = child;
}

static void go(int n)
{
 for (;n>0;n--) write(gopipe[1],"",1);
 close(gopipe[0]);
 close(gopipe[1]);
}

static void p1main(int comm)
{
 printf("p1 started, pid %d\n",getpid());
 write(comm,"",1);
 while (1)
  { printf("p1 tick\n");
    sleep(5);
  }
}

static void p2main(int comm)
{
 pid_t v;
 int i;
 pid_t w;
 int status;
 int s;

 printf("p2 started, pid %d\n",getpid());
 for (i=0;i<sizeof(pid_t);i++) if (read(comm,((char *)&v)+i,1) != 1) exit(1);
 printf("p2 attaching to %d\n",(int)v);
 if (ptrace(PT_ATTACH,v,0,0) < 0)
  { printf("p2 PT_ATTACH failed: %s\n",strerror(errno));
    exit(1);
  }
 printf("p2 waiting for %d to stop\n",(int)v);
 w = wait4(v,&status,0,0);
 if (w < 0)
  { printf("p2 stop wait errored: %s\n",strerror(errno));
    exit(1);
  }
 if (w == 0)
  { printf("p2 stop wait indicated no children(?)\n");
    exit(1);
  }
 if (w != v)
  { printf("p2 stop wait returned unexpected pid %d\n",(int)w);
    exit(1);
  }
 if (! WIFSTOPPED(status))
  { printf("p2 stop wait indicates non-stop\n");
    exit(1);
  }
 printf("p2 stop wait shows signal %d\n",WSTOPSIG(status));
 printf("p2 telling %d to continue\n",(int)v);
 if (ptrace(PT_CONTINUE,v,(caddr_t)1,0) < 0) /* XXX interface botch */
  { printf("p2 PT_CONTINUE failed: %s\n",strerror(errno));
    exit(1);
  }
 write(comm,"",1);
 while (1)
  { printf("p2 waiting for %d to stop\n",(int)v);
    w = wait4(v,&status,0,0);
    if (w < 0)
     { printf("p2 stop wait errored: %s\n",strerror(errno));
       exit(1);
     }
    if (w == 0)
     { printf("p2 stop wait indicated no children(?)\n");
       exit(1);
     }
    if (w != v)
     { printf("p2 stop wait returned unexpected pid %d\n",(int)w);
       exit(1);
     }
    if (! WIFSTOPPED(status))
     { printf("p2 stop wait indicates non-stop\n");
       exit(1);
     }
    s = WSTOPSIG(status);
    printf("p2 stop wait shows signal %d\n",s);
    if (s == 9) s = 0;
    printf("p2 telling %d to continue, signal %d\n",(int)v,s);
    if (ptrace(PT_CONTINUE,v,(caddr_t)1,s) < 0) /* XXX interface botch */
     { printf("p2 PT_CONTINUE failed: %s\n",strerror(errno));
       exit(1);
     }
  }
}

static void p3main(int comm)
{
 pid_t v;
 int i;

 printf("p3 started, pid %d\n",getpid());
 for (i=0;i<sizeof(pid_t);i++) if (read(comm,((char *)&v)+i,1) != 1) exit(1);
 printf("p3 SIGKILLing %d\n",(int)v);
 kill(v,SIGKILL);
 printf("p3 sent SIGKILL\n");
 write(comm,"",1);
}

static void die(void) __attribute__((__noreturn__));
static void die(void)
{
 kill(k1.k,SIGKILL);
 kill(k2.k,SIGKILL);
 kill(k3.k,SIGKILL);
 exit(1);
}

static void read1(KID *k)
{
 char junk;
 int rrv;

 rrv = read(k->p,&junk,1);
 switch (rrv)
  { case 1:
       return;
       break;
    case 0:
       printf("protocol error, unexpected EOF from %s (%d)\n",k->tag,(int)k->k);
       die();
       break;
    case -1:
       printf("read error (%s) from %s (%d)\n",strerror(errno),k->tag,(int)k->k);
       die();
       break;
  }
 printf("unexpected read return %d from %s (%d)\n",rrv,k->tag,(int)k->k);
 die();
}

int main(void);
int main(void)
{
 if (pipe(gopipe) < 0)
  { printf("pipe: %s\n",strerror(errno));
    exit(1);
  }
 checked_fork("p1",p1main,&k1);
 checked_fork("p2",p2main,&k2);
 checked_fork("p3",p3main,&k3);
 go(3);
 /* Wait for p1 to get started (probably not strictly necessary) */
 read1(&k1);
 /* Tell p2 to trace p1 */
 write(k2.p,&k1.k,sizeof(pid_t));
 /* Wait for p2 to be done with it */
 read1(&k2);
 /* Tell p3 to SIGKILL p1 */
 write(k3.p,&k1.k,sizeof(pid_t));
 /* Wait for a while for ticks to happen */
 sleep(20);
 die();
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
