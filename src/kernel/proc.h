#ifndef PROC_H
#define PROC_H

#include <csolve.h>
#include <unistd.h>

#define UI_PROC  "../procs/output.py"
#define TAB_PROC "../procs/tab.py"
#define COOKIE_PROC "../procs/cookiep.py"

void call(const char *path, char *const argv[]);
pid_t run_proc(const char NULLTERMSTR FINAL *STRINGPTR file,
               char NULLTERMSTR FINAL *NNSTRINGPTR FINAL
                                      *START ARRAY argv) OKEXTERN;
#endif
