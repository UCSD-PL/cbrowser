#ifndef BROWSER_COOKIE_UTIL
#define BROWSER_COOKIE_UTIL

#include <csolve.h>
#include <stdlib.h>
#include "constants.h"

#define nnparse_string NULLTERMSTR CSOLVE_DOMAIN_STR * NNSTRINGPTR
#define parse_string NULLTERMSTR CSOLVE_DOMAIN_STR * STRINGPTR

struct cookie {
  char parse_string REF(DOMAIN([V]) = THE_STRING([V])) FINAL domain; /*simplification*/
  char parse_string attrs;
  char parse_string path;
  int  httpOnly;
};

struct get_cookie {
  char parse_string NNREF(DOMAIN([V]) = THE_STRING([V])) domain;
  char parse_string scheme;
  char parse_string path;
};

void
may_get_cookie(char FINAL parse_string REF(? COOKIE_DOMAIN_GET([DOMAIN([V]);DOMAIN([c])])) domain,
               struct cookie FINAL *c) OKEXTERN;

struct cookie FINAL * NNSTART NNVALIDPTR NNROOM_FOR(struct cookie) NNREF(TAGSET([V]) = TAGSET([cookie_str])) NNREF(DOMAIN([V]) = DOMAIN([DEREF([V])]))
parse_cookie(const char CSOLVE_DOMAIN_STR NULLTERMSTR FINAL * STRINGPTR cookie_str, size_t n) OKEXTERN;

struct cookie FINAL * NNSTART NNVALIDPTR NNROOM_FOR(struct get_cookie) NNREF(TAGSET([V]) = TAGSET([get_cookie_str])) NNREF(DOMAIN([V]) = DOMAIN([DEREF([V])]))
parse_get_cookie(const char CSOLVE_DOMAIN_STR NULLTERMSTR FINAL * STRINGPTR get_cookie_str,
                 size_t n) OKEXTERN;

void
print_cookie(struct cookie *c) OKEXTERN;

char CSOLVE_DOMAIN_STR NULLTERMSTR * START STRINGPTR REF(DOMAIN([V]) = DOMAIN([DEREF([s])]))
serialize_cookie(struct cookie FINAL *s) OKEXTERN;

//domain d1 may get cookies from d2
int REF(V != 0 => ? COOKIE_DOMAIN_SET([DOMAIN([d1]);DOMAIN([d2])]))
may_set_cookies(char FINAL parse_string d1, char FINAL parse_string d2) OKEXTERN;

//domain d1 may set cookies on d2
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

#endif
