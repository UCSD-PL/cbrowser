#ifndef PROC_H
#define PROC_H

#include <csolve.h>
#include <unistd.h>

#define TAB_PROC "../../../trusted/src/procs/tab.py"
#define UI_PROC  "../../../trusted/src/procs/output.py"

void
call(const char *path, char *const argv[]);

void
init_piped_process(const char NULLTERMSTR * STRINGPTR procfile,
                   char       NULLTERMSTR * NNSTRINGPTR * START ARRAY VALIDPTR SIZE_GE(8) args,
                   pid_t *proc,
                   int   *REF(V > 0 => ? Set_emp([TAGSET([V])])) soc)
  OKEXTERN;

#endif
