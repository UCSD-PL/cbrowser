#include <csolve.h>
#include <sys/types.h>
#include <sys/socket.h>

#define UI_PROC  "../procs/output.py"

void
ui_init()
  OKEXTERN;

void
ui_kill(int sig)
  OKEXTERN;

pid_t
ui_proc()
  OKEXTERN;

int REF(V > 0 => ? Set_emp([TAGSET([V])]))
ui_soc()
  OKEXTERN;
