#include <csolve.h>
#include "cookie_proc.h"

void assert_eq_strings(char NULLTERMSTR FINAL *STRINGPTR REF(THE_STRING([V]) = THE_STRING([s2])) s1,
                       char NULLTERMSTR FINAL *STRINGPTR REF(THE_STRING([s1]) = THE_STRING([V])) s2) OKEXTERN;


void
cond_set_cookie(char NULLTERMSTR FINAL * START STRINGPTR s, char NULLTERMSTR FINAL * START STRINGPTR t) CHECK_TYPE
{
  struct cookie_proc *cp;

  cp = get_cookie_process(s);
  if (cp) {
    assert_eq_strings(cp->cookie_origin, s);
  }
}
