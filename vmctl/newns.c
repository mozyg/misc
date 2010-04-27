#include <sched.h>
#include <syscall.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

#ifndef CLONE_NEWNS
#define CLONE_NEWNS 0x00020000
#endif

#ifndef CLONE_NEWPID
#define CLONE_NEWPID   0x20000000      /* New pid namespace.  */
#endif

#ifndef CLONE_NEWIPC
#define CLONE_NEWIPC	0x08000000	/* New ipcs.  */
#endif

#ifndef CLONE_NEWUTS
#define CLONE_NEWUTS	0x04000000	/* New utsname group.  */
#endif

#ifndef CLONE_NEWUSER
#define CLONE_NEWUSER	0x10000000	/* New user namespace.  */
#endif

int main(int argc, char *argv[]) {
  int ret = 0;
  int pid = syscall(SYS_clone, CLONE_NEWUSER | CLONE_NEWPID | CLONE_NEWNS |
	      CLONE_NEWUTS | CLONE_NEWIPC | SIGCHLD,0);
  if (pid != 0) {
          waitpid(pid, &ret, 0);
          exit(-1);
  } else {
	if (argc > 1) {
	    execlp(argv[1], argv[1], NULL);
	} else {
	    execlp("/bin/sh", "/bin/sh",NULL);
	}
	exit(-1);
  }
}
