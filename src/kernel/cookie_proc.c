#include "cookie_proc.h"
#include "proc.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <search.h>


void *cookie_proc_tree = NULL;

int
cookie_proc_compare(const void * LOC(L) LAYOUT(struct cookie_proc) cp1,
                    const void * LOC(L) LAYOUT(struct cookie_proc) cp2) CHECK_TYPE
{
  struct cookie_proc *c1 = (struct cookie_proc *)cp1;
  struct cookie_proc *c2 = (struct cookie_proc *)cp2;
  return strcmp(c1->cookie_origin, c2->cookie_origin);
}

struct cookie_proc REF(DOMAIN([cookie_origin]) = THE_STRING([domain])) *
NNSTART NNVALIDPTR NNROOM_FOR(struct cookie_proc)
  get_cookie_process(char NULLTERMSTR * STRINGPTR domain) CHECK_TYPE
{
  struct cookie_proc key;
  struct cookie_proc **retval;

  key.cookie_origin = strdup(domain);
  if (cookie_proc_tree) {
    retval = tfind(&key, &cookie_proc_tree, cookie_proc_compare);
    assert (retval);
    return *retval;
  }
  return NULL;
}

void
init_cookie_process(char NULLTERMSTR * STRINGPTR origin) CHECK_TYPE
{
  //Open a socket for this process
  //add struct into hash search table
  int soc;
  pid_t proc;
  struct cookie_proc *cookie_proc;
  char *args[5];
  assert (origin != NULL);

  cookie_proc = malloc(sizeof(*cookie_proc));
  cookie_proc->cookie_origin = strdup(origin);
  args[2] = strdup(origin);
  init_piped_process(COOKIE_PROC,
                     args,
                     &proc,
                     &soc);
  cookie_proc->proc = proc;
  cookie_proc->soc  = soc;


  //Should check the return value here
  tsearch((void *)cookie_proc,
          &cookie_proc_tree,
          cookie_proc_compare);
}
