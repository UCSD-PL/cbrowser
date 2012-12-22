#include "util.h"

char NULLTERMSTR DOMAIN_STR * LOC(S1)
START STRINGPTR
REF(? COOKIE_DOMAIN_GET([DOMAIN([d]);DOMAIN([V])]))
strapp(char NULLTERMSTR DOMAIN_STR * STRINGPTR d,
       char NULLTERMSTR DOMAIN_STR * LOC(S1) START STRINGPTR REF(? COOKIE_DOMAIN_GET([DOMAIN([d]);DOMAIN([V])])) s1,
       char NULLTERMSTR DOMAIN_STR * START STRINGPTR REF(? COOKIE_DOMAIN_GET([DOMAIN([d]);DOMAIN([V])])) s2)
CHECK_TYPE
{
  size_t sz = strlen(s1) + strlen(s2);

  s1 = realloc(s1, sz);
  return strncat(s1, s2, sz);
}
