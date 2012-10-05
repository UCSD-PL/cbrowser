#include "proc.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>



void call(const char *path, char *const argv[]) {
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


pid_t run_proc(const char *file, char *const argv[]) {
    pid_t pid;
    int status;
    
    ///////
    //int i = 0;
    //while (argv[i]) {
    //    printf("%d: %s\n", i, argv[i]);
    //    i++;
    //}
    ///////
    
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