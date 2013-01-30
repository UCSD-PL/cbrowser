#include "util.h"

char NULLTERMSTR ICHAR * LOC(S1)
START STRINGPTR
REF(? COOKIE_DOMAIN_GET([DOMAIN([d]);DOMAIN([V])]))
strapp(char NULLTERMSTR ICHAR * STRINGPTR d,
       char NULLTERMSTR ICHAR * LOC(S1) START STRINGPTR REF(? COOKIE_DOMAIN_GET([DOMAIN([d]);DOMAIN([V])])) s1,
       char NULLTERMSTR ICHAR * START STRINGPTR REF(? COOKIE_DOMAIN_GET([DOMAIN([d]);DOMAIN([V])])) s2)
CHECK_TYPE
{
  size_t sz = strlen(s1) + strlen(s2);

  s1 = realloc(s1, sz);
  return strncat(s1, s2, sz);
}

char *
immutable_strdup(char * __s)
{
  return strdup(__s);
}

char *
mutable_strdup(char * __s)
{
  return strdup(__s);
}

char ICHAR NULLTERMSTR *
START STRINGPTR
REF(THE_STRING([V]) = THE_STRING([__s]))
REF(DOMAIN([V])     = DOMAIN([__s]))
REF(TAGSET([V])     = TAGSET([__s]))
  strdupi(char NULLTERMSTR ICHAR FINAL * STRINGPTR __s)
{
  return strdup(__s);
}
