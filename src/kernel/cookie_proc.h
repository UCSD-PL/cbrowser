#include <csolve.h>
#include <string.h>
#include <sys/types.h>

/* struct cookie_proc INST(S, COOKIE_PROC_TREE_LOC) * LOC(COOKIE_PROC_TREE_STRUCT) cookie_proc_tree = NULL; */

struct cookie_proc {
  char NULLTERMSTR CSOLVE_DOMAIN_STR * LOC(S) STRINGPTR REF(DOMAIN([V]) = THE_STRING([V])) cookie_origin;
  pid_t proc;
  int REF(DOMAIN([V]) = DOMAIN([cookie_origin])) soc;
};

void init_cookie_process(char NULLTERMSTR * STRINGPTR origin) OKEXTERN;

struct cookie_proc REF(DOMAIN([cookie_origin]) = THE_STRING([domain])) *
NNSTART NNVALIDPTR NNROOM_FOR(struct cookie_proc)
get_cookie_process(char NULLTERMSTR * STRINGPTR domain) OKEXTERN;

