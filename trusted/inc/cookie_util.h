#ifndef BROWSER_COOKIE_UTIL
#define BROWSER_COOKIE_UTIL

#include <csolve.h>
#include <stdlib.h>
#include "constants.h"

struct cookie {
  char parse_string REF(DOMAIN([V]) = THE_STRING([V])) FINAL domain; /*simplification*/
  char parse_string attrs;
  char parse_string path;
  int  httpOnly;
};

void
may_get_cookie(char FINAL parse_string REF(? COOKIE_DOMAIN_GET([DOMAIN([V]);DOMAIN([c])])) domain,
               struct cookie FINAL *c) OKEXTERN;

char NULLTERMSTR DOMAIN_STR
* ARRAY NNSTART NNVALIDPTR REF((V != 0) => ((VVADDR + 8) < BLOCK_END([VVADDR]))) NNREF(? COOKIE_DOMAIN_GET([DOMAIN([d]);DOMAIN([V])])) REF(DOMAIN([V]) = THE_STRING([V])) FINAL
* ARRAY START VALIDPTR SIZE_GE(4) REF(? COOKIE_DOMAIN_GET([DOMAIN([d]);DOMAIN([DEREF([V])])]))
  gettable_domains(char FINAL NULLTERMSTR DOMAIN_STR * STRINGPTR REF(DOMAIN([V]) = THE_STRING([V])) d) OKEXTERN;

struct cookie FINAL * NNSTART NNVALIDPTR NNROOM_FOR(struct cookie) NNREF(TAGSET([V]) = TAGSET([cookie_str])) NNREF(DOMAIN([V]) = DOMAIN([DEREF([V])]))
parse_cookie(const char DOMAIN_STR NULLTERMSTR FINAL * STRINGPTR cookie_str, size_t n) OKEXTERN;


void
print_cookie(struct cookie *c) OKEXTERN;

char DOMAIN_STR NULLTERMSTR * START STRINGPTR REF(DOMAIN([V]) = DOMAIN([DEREF([s])]))
serialize_cookie(struct cookie FINAL *s) OKEXTERN;

//domain d1 may set cookies from d2
int REF(V != 0 => ? COOKIE_DOMAIN_SET([DOMAIN([d1]);DOMAIN([d2])]))
may_set_cookies(char FINAL parse_string d1, char FINAL parse_string d2) OKEXTERN;

//domain d1 may get cookies on d2
int REF(V != 0 => ? COOKIE_DOMAIN_GET([DOMAIN([d1]);DOMAIN([d2])]))
may_get_cookies(char FINAL parse_string d1, char FINAL parse_string d2) OKEXTERN;

void
assert_may_set(char FINAL parse_string REF(? COOKIE_DOMAIN_SET([DOMAIN([V]); DOMAIN([d2])])) d1,
               char FINAL parse_string REF(? COOKIE_DOMAIN_SET([DOMAIN([d1]); DOMAIN([V])])) d2) OKEXTERN;

void
assert_may_set2(int REF(? COOKIE_DOMAIN_SET([DOMAIN([V]); DOMAIN([d2])])) d1,
               char FINAL parse_string REF(? COOKIE_DOMAIN_SET([DOMAIN([d1]); DOMAIN([V])])) d2) OKEXTERN;

void
assert_may_get(char FINAL parse_string REF(? COOKIE_DOMAIN_GET([DOMAIN([V]); DOMAIN([d2])])) d1,
               char FINAL parse_string REF(? COOKIE_DOMAIN_GET([DOMAIN([d1]); DOMAIN([V])])) d2) OKEXTERN;
void
assert_may_get2(int REF(? COOKIE_DOMAIN_GET([DOMAIN([V]); DOMAIN([d2])])) d1,
                char FINAL parse_string REF(? COOKIE_DOMAIN_GET([DOMAIN([d1]); DOMAIN([V])])) d2) OKEXTERN;

char NULLTERMSTR DOMAIN_STR *
START
STRINGPTR
REF(? COOKIE_DOMAIN_GET([DOMAIN([s1]);DOMAIN([V])])) LOC(S)
REF(THE_STRING([V]) = THE_STRING([s3]))
read_is_transitive(char FINAL NULLTERMSTR DOMAIN_STR * STRINGPTR s1,
                   char FINAL NULLTERMSTR DOMAIN_STR * 
                   STRINGPTR REF(? COOKIE_DOMAIN_GET([DOMAIN([s1]);DOMAIN([V])])) s2,
                   char FINAL NULLTERMSTR DOMAIN_STR *
                   LOC(S) STRINGPTR REF(? COOKIE_DOMAIN_GET([DOMAIN([s2]);DOMAIN([V])])) s3)
                    OKEXTERN;

#endif
