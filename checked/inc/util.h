#ifndef UTIL
#define UTIL
#include <csolve.h>
#include <string.h>
#include <stdlib.h>
#include "constants.h"
#include "types.h"

char ICHAR NULLTERMSTR *
LOC(S1)
STRINGPTR
REF(? COOKIE_DOMAIN_GET([DOMAIN([d]);DOMAIN([V])]))
strapp(char ICHAR NULLTERMSTR FINAL * STRINGPTR d,
       char ICHAR NULLTERMSTR FINAL * LOC(S1) STRINGPTR REF(? COOKIE_DOMAIN_GET([DOMAIN([d]);DOMAIN([V])])) s1,
       char ICHAR NULLTERMSTR FINAL * STRINGPTR REF(? COOKIE_DOMAIN_GET([DOMAIN([d]);DOMAIN([V])])) s2) OKEXTERN;

char NULLTERMSTR ICHAR *
START STRINGPTR
REF(THE_STRING([V]) = THE_STRING([__s]))
REF(DOMAIN([V])     = DOMAIN([__s]))
REF(TAGSET([V])     = TAGSET([__s]))
  immutable_strdup(char NULLTERMSTR FINAL * STRINGPTR __s)
OKEXTERN;

char NULLTERMSTR *
START STRINGPTR
REF(THE_STRING([V]) = THE_STRING([__s]))
REF(DOMAIN([V])     = DOMAIN([__s]))
REF(TAGSET([V])     = TAGSET([__s]))
  mutable_strdup(char NULLTERMSTR ICHAR FINAL * STRINGPTR __s)
OKEXTERN;

char ICHAR NULLTERMSTR *
START STRINGPTR
REF(THE_STRING([V]) = THE_STRING([__s]))
REF(DOMAIN([V])     = DOMAIN([__s]))
REF(TAGSET([V])     = TAGSET([__s]))
  strdupi(char NULLTERMSTR ICHAR FINAL * STRINGPTR __s)
OKEXTERN;


#endif

