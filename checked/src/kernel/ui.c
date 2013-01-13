#include <csolve.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "ui.h"
#include "proc.h"
#include "tags.h"

struct uiproc {
  pid_t proc;
  int   REF(V > 0 => ? Set_emp([TAGSET([V])])) soc;
};

struct uiproc ui CHECK_TYPE;

void
ui_init() CHECK_TYPE
{
  char *args[5];
  pid_t proc;
  int soc;

  args[2] = NULL;
  init_piped_process(UI_PROC,
                     args,
                     &proc,
                     &soc);
  ui.proc = proc;
  ui.soc = soc;
}

void
ui_kill(int sig) 
{
  kill(ui.proc, sig);
}

int REF(V > 0 => ? Set_emp([TAGSET([V])]))
ui_soc() CHECK_TYPE
{
  return ui.soc;
}

pid_t
ui_proc()
{
  return ui.proc;
}
