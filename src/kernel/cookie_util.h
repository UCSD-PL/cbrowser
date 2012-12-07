#include <csolve.h>
#include <stdlib.h>
#include "constants.h"

#define nnparse_string NULLTERMSTR CSOLVE_DOMAIN_STR * NNSTRINGPTR
#define parse_string NULLTERMSTR CSOLVE_DOMAIN_STR * STRINGPTR

struct cookie {
  char nnparse_string NNREF(DOMAIN([V]) = THE_STRING([V])) domain; /*simplification*/
  char nnparse_string attrs;
  char nnparse_string path;
  int  httpOnly;
};

struct get_cookie {
  char nnparse_string NNREF(DOMAIN([V]) = THE_STRING([V])) domain;
  char nnparse_string scheme;
  char nnparse_string path;
};

struct res_cookie {
  /* a res_cookie is a message to a specific tab, `des' */
  int    REF(V >= 0) REF(V < MAX_NUM_TABS) dest;
  int    REF(V >= 0) num_cookies;
  /* the domain for which cookies were originall requested */
  char   nnparse_string NNREF(DOMAIN([V]) = THE_STRING([V])) FINAL domain;
  struct cookie * START VALIDPTR ROOM_FOR(struct cookie) FINAL
    * ARRAY NNSTART NNVALIDPTR NNSIZE_GE(4*num_cookies)
      NNREF(DOMAIN([DEREF([V])]) = DOMAIN([domain]))
      FINAL
    cookies;
};

struct cookie_jar {
  struct cookie *c;
  struct cookie_jar *next_jar;
};


struct cookie FINAL * NNSTART NNVALIDPTR NNROOM_FOR(struct cookie) NNREF(TAGSET([V]) = TAGSET([cookie_str])) NNREF(DOMAIN([V]) = DOMAIN([DEREF([V])]))
parse_cookie(const char CSOLVE_DOMAIN_STR NULLTERMSTR FINAL * STRINGPTR cookie_str, size_t n) OKEXTERN;

int
parse_get_cookie(struct get_cookie *,
                 const char CSOLVE_DOMAIN_STR NULLTERMSTR FINAL * STRINGPTR get_cookie_str,
                 size_t n) OKEXTERN;

int
parse_res_cookie(struct res_cookie *,
                 const char CSOLVE_DOMAIN_STR NULLTERMSTR FINAL * STRINGPTR res_cookie_str,
                 size_t n) OKEXTERN;

void
print_cookie(struct cookie *c) OKEXTERN;

char CSOLVE_DOMAIN_STR NULLTERMSTR * START STRINGPTR REF(DOMAIN([V]) = DOMAIN([DEREF([s])]))
serialize_cookie(struct cookie FINAL *s) OKEXTERN;

/* char CSOLVE_DOMAIN_STR NULLTERMSTR * START STRINGPTR REF(DOMAIN([V]) = DOMAIN([DEREF([cookies])])) */
/* serialize_cookie_list(int REF(V >= 0) num_cookies, */
/*                       struct cookie FINAL * FINAL * ARRAY START VALIDPTR SIZE_GE(4*num_cookies) cookies) */
/*   OKEXTERN; */

void
set_cookie(struct cookie_jar **cookies, struct cookie *c);

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
assert_may_get2(char FINAL parse_string REF(? COOKIE_DOMAIN_GET([THE_STRING([V]); DOMAIN([d2])])) d1,
               char FINAL parse_string REF(? COOKIE_DOMAIN_GET([THE_STRING([d1]); DOMAIN([V])])) d2) OKEXTERN;
