#ifndef UTIL
#define UTIL
#include <csolve.h>
#include <string.h>
#include <stdlib.h>
#include "constants.h"
#include "types.h"

/* char Immutable LOC(S1) REF(? COOKIE_DOMAIN_GET([DOMAIN([d]);DOMAIN([V])])) */
/* strapp(char FINAL Immutable d, */
/*        char FINAL Immutable REF(? COOKIE_DOMAIN_GET([DOMAIN([d]);DOMAIN([V])])) LOC(S1) s1, */
/*        char FINAL NImmutable NNREF(? COOKIE_DOMAIN_GET([DOMAIN([d]);DOMAIN([V])])) LOC(S1) s2) OKEXTERN; */

char Immutable LOC(S1) REF(|| [DOMAIN([V]) = DOMAIN([s1]); DOMAIN([V]) = DOMAIN([s2])]) 
                       REF(|| [SOURCE([V]) = SOURCE([s1]); SOURCE([V]) = SOURCE([s2])]) 
strapp(char FINAL Immutable d,
       char FINAL Immutable LOC(S1) s1,
       char FINAL NImmutable LOC(S1) s2) OKEXTERN;

char NULLTERMSTR * I
START STRINGPTR
REF(DOMAIN([V])     = DOMAIN([__s]))
  immutable_strdup(char NULLTERMSTR FINAL * STRINGPTR __s)
OKEXTERN;

char NULLTERMSTR *
START STRINGPTR
REF(DOMAIN([V])     = DOMAIN([__s]))
  mutable_strdup(char NULLTERMSTR FINAL * I STRINGPTR __s)
OKEXTERN;

char NULLTERMSTR * I
START STRINGPTR
REF(DOMAIN([V])     = DOMAIN([__s]))
REF(THE_STRING([V])     = THE_STRING([__s]))
  strdupi(char NULLTERMSTR FINAL * I STRINGPTR __s)
OKEXTERN;


#endif


