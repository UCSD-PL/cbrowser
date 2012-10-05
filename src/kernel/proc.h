#ifndef PROC_H
#define PROC_H

#include <unistd.h>

#define UI_PROC  "../procs/output.py"
#define TAB_PROC "../procs/tab.py"

void call(const char *path, char *const argv[]);
pid_t run_proc(const char *file, char *const argv[]);


#endif
