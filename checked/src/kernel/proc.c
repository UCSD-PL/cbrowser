#include "proc.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>

void call(const char *path, char *const argv[])
{
    pid_t pid;
    int status;
    if ( (pid = fork()) == 0) { // child
        execvp(path, argv);
    } else if (pid > 0) { // parent
        waitpid(pid, &status, 0);
    } else { // error
        //TODO: report some error
    }
}

pid_t run_proc(const char NULLTERMSTR FINAL *STRINGPTR file,
               char NULLTERMSTR FINAL *NNSTRINGPTR FINAL
                                      *START ARRAY argv)
{
    pid_t pid;
    int status;
    
    if ( (pid = fork()) == 0) { // child
        execvp(file, argv);
    } else if (pid > 0) { // parent
        //wait(&status); //wait WNOHANG? is waiting necessary?
        waitpid(pid, &status, WNOHANG);
        //printf("exited normally: %d\n", WIFEXITED(status));
    } else { // error
        //TODO: report some error
    }
    return pid;
}

void
init_piped_process(const char NULLTERMSTR * STRINGPTR procfile,
                   char       NULLTERMSTR * STRINGPTR * START ARRAY VALIDPTR SIZE_GE(12) args,
                   pid_t *proc,
                   int   REF(V > 0 => Set_emp([TAGSET([V])])) *soc)
/* void */
/* init_piped_process(char *procfile, char **args, pid_t *proc, int *soc) */
{
  int socs[2];
  char child[16];
  if (socketpair(AF_UNIX, SOCK_STREAM, 0, socs) < 0) {
    //TODO: print some error
  }

  // Add the child socket to the args list
  //  Note: the caller of this function must setup args appropriately
  //      - leave the first space in args empty
  //      - list any args specific to the process
  //      - list the command line args
  //      - NULL
  sprintf(child, "%d", socs[1]);
  args[0] = strdup(procfile);
  args[1] = child;

  *proc = run_proc(procfile, args);
  *soc  = socs[0];
}
