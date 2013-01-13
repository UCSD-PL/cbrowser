#include <csolve.h>
#include <sys/types.h>
#include "constants.h"
#include "tags.h"

struct tab {
  char NULLTERMSTR ICHAR * FINAL
  LOC(L) START STRINGPTR REF(DOMAIN([V]) = THE_STRING([V])) REF(TAGSET([V]) = TAGSET([soc])) tab_origin;
  pid_t proc;                //PID of tab process
  int   FINAL REF(TAGSET([V]) = Set_sng([V])) REF(DOMAIN([V]) = DOMAIN([tab_origin])) soc; //socket
};

#define NNKERNEL_TABS struct tab * NNSTART NNVALIDPTR NNROOM_FOR(struct tab) * ARRAY NNSTART NNVALIDPTR NNSIZE_GE(40) 
#define KERNEL_TABS struct tab * NNSTART NNVALIDPTR NNROOM_FOR(struct tab) * ARRAY START VALIDPTR SIZE_GE(40) 

#define KERNEL_TABS_FINAL struct tab FINAL * NNSTART NNVALIDPTR NNROOM_FOR(struct tab) FINAL * ARRAY START VALIDPTR SIZE_GE(40) 

void init_tab_process(KERNEL_TABS tabx, int tab_idx, char NULLTERMSTR * STRINGPTR init) OKEXTERN; 
void tab_kill(KERNEL_TABS_FINAL tabs, int tab_idx, int signal) OKEXTERN;
int  tab_fd(KERNEL_TABS_FINAL tabs, int tab_idx) OKEXTERN;
char NULLTERMSTR* NNSTART NNSTRINGPTR tab_title(KERNEL_TABS_FINAL tabs,  int tab_idx) OKEXTERN;
